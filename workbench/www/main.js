"use strict";

const canvas = document.querySelector("#viewport");
const form = document.querySelector("#generator");
const seed = document.querySelector("#seed");
const status = document.querySelector("#status");
const stage = document.querySelector(".stage");
const download = document.querySelector("#download");
const reset = document.querySelector("#reset-view");
const generateButton = form.querySelector("[type=submit]");

function decodeScene(buffer) {
  const header = new DataView(buffer);
  const jsonLength = header.getUint32(12, true);
  const binOffset = 28 + jsonLength;
  if (header.getUint32(0, true) !== 0x46546c67 ||
      header.getUint32(4, true) !== 2 || header.getUint32(8, true) !== buffer.byteLength ||
      header.getUint32(16, true) !== 0x4e4f534a ||
      header.getUint32(binOffset - 4, true) !== 0x004e4942) {
    throw new Error("The generator returned an unreadable GLB.");
  }
  const json = JSON.parse(new TextDecoder().decode(new Uint8Array(buffer, 20, jsonLength)));
  const scene = json.scenes[json.scene];
  const min = [Infinity, Infinity, Infinity];
  const max = [-Infinity, -Infinity, -Infinity];
  const parts = [];

  // GeoRena writes untransformed nodes and interleaved float positions and normals.
  for (const nodeIndex of scene.nodes) {
    const mesh = json.meshes[json.nodes[nodeIndex].mesh];
    for (const part of mesh.primitives) {
      const position = json.accessors[part.attributes.POSITION];
      const view = json.bufferViews[position.bufferView];
      for (let axis = 0; axis < 3; axis++) {
        min[axis] = Math.min(min[axis], position.min[axis]);
        max[axis] = Math.max(max[axis], position.max[axis]);
      }
      const material = json.materials[part.material].pbrMetallicRoughness;
      parts.push({
        start: view.byteOffset / view.byteStride,
        count: position.count,
        color: material.baseColorFactor.slice(0, 3)
      });
    }
  }

  return {
    bytes: new Uint8Array(buffer, binOffset),
    parts,
    objects: scene.nodes.length,
    triangles: parts.reduce((total, part) => total + part.count / 3, 0),
    center: min.map((value, axis) => (value + max[axis]) / 2),
    radius: Math.hypot(...min.map((value, axis) => (max[axis] - value) / 2)),
    ...scene.extras
  };
}

function makeViewer(gl) {
  const vertex = `
    attribute vec3 position;
    attribute vec3 normal;
    uniform vec3 eye, right, up, forward;
    uniform float aspect;
    varying float light;
    void main() {
      vec3 delta = position - eye;
      float depth = dot(delta, forward);
      gl_Position = vec4(2.3 * dot(delta, right) / aspect,
                         2.3 * dot(delta, up), 1.01 * depth - 10.05, depth);
      light = 0.49 + 0.51 * max(0.0, dot(normal, normalize(vec3(-0.52, 0.85, 0.31))));
    }`;
  const fragment = `
    precision mediump float;
    varying float light;
    uniform vec3 paint;
    void main() {
      gl_FragColor = vec4(sqrt(paint * light), 1.0);
    }`;

  const program = gl.createProgram();
  for (const [type, source] of [[gl.VERTEX_SHADER, vertex], [gl.FRAGMENT_SHADER, fragment]]) {
    const shader = gl.createShader(type);
    gl.shaderSource(shader, source);
    gl.compileShader(shader);
    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
      throw new Error("The browser could not compile the preview renderer.");
    }
    gl.attachShader(program, shader);
    gl.deleteShader(shader);
  }
  gl.linkProgram(program);
  if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
    throw new Error("The browser could not start the preview renderer.");
  }
  gl.useProgram(program);
  const uniforms = Object.fromEntries(["eye", "right", "up", "forward", "aspect", "paint"]
    .map(name => [name, gl.getUniformLocation(program, name)]));
  const vertices = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, vertices);
  for (const [name, offset] of [["position", 0], ["normal", 12]]) {
    const index = gl.getAttribLocation(program, name);
    gl.enableVertexAttribArray(index);
    gl.vertexAttribPointer(index, 3, gl.FLOAT, false, 24, offset);
  }
  gl.enable(gl.DEPTH_TEST);
  gl.enable(gl.CULL_FACE);

  let scene, yaw, pitch, zoom;
  const cross = ([ax, ay, az], [bx, by, bz]) =>
    [ay * bz - az * by, az * bx - ax * bz, ax * by - ay * bx];

  function draw() {
    if (!scene) return;
    const {width, height} = canvas.getBoundingClientRect();
    if (width < 1 || height < 1) return;
    const ratio = Math.min(window.devicePixelRatio || 1, 2);
    const pixelsWide = Math.round(width * ratio);
    const pixelsHigh = Math.round(height * ratio);
    if (canvas.width !== pixelsWide || canvas.height !== pixelsHigh) {
      canvas.width = pixelsWide;
      canvas.height = pixelsHigh;
    }
    gl.viewport(0, 0, pixelsWide, pixelsHigh);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    const aspect = pixelsWide / pixelsHigh;
    const framing = scene.mode === "fps" ? 2.16 : 1.86;
    const distance = scene.radius * framing * zoom * Math.max(1, 1.2 / aspect);
    const forward = [-Math.sin(yaw) * Math.cos(pitch), -Math.sin(pitch),
                     -Math.cos(yaw) * Math.cos(pitch)];
    const right = [Math.cos(yaw), 0, -Math.sin(yaw)];
    const up = cross(right, forward);
    const eye = scene.center.map((value, axis) => value - distance * forward[axis]);
    gl.uniform3fv(uniforms.eye, eye);
    gl.uniform3fv(uniforms.right, right);
    gl.uniform3fv(uniforms.up, up);
    gl.uniform3fv(uniforms.forward, forward);
    gl.uniform1f(uniforms.aspect, aspect);
    for (const {start, count, color} of scene.parts) {
      gl.uniform3fv(uniforms.paint, color);
      gl.drawArrays(gl.TRIANGLES, start, count);
    }
  }

  function resetCamera() {
    yaw = 0.53;
    pitch = 0.76;
    zoom = 1;
    draw();
  }

  function orbit(x, y) {
    yaw -= x * 0.008;
    pitch = Math.max(0.03, Math.min(1.54, pitch + y * 0.008));
    draw();
  }

  function dolly(factor) {
    zoom = Math.max(0.35, Math.min(3.1, zoom * factor));
    draw();
  }

  let drag = null;
  canvas.addEventListener("pointerdown", event => {
    if (!scene || drag || event.button !== 0) return;
    drag = {id: event.pointerId, x: event.clientX, y: event.clientY};
    canvas.setPointerCapture(drag.id);
  });
  canvas.addEventListener("pointermove", event => {
    if (event.pointerId !== drag?.id) return;
    orbit(event.clientX - drag.x, event.clientY - drag.y);
    drag = {id: drag.id, x: event.clientX, y: event.clientY};
  });
  canvas.addEventListener("lostpointercapture", () => { drag = null; });
  canvas.addEventListener("wheel", event => {
    if (!scene) return;
    event.preventDefault();
    dolly(event.deltaY > 0 ? 1.10 : 0.91);
  }, {passive: false});
  canvas.addEventListener("keydown", event => {
    if (!scene) return;
    switch (event.key) {
      case "ArrowLeft": orbit(-22, 0); break;
      case "ArrowRight": orbit(22, 0); break;
      case "ArrowUp": orbit(0, 22); break;
      case "ArrowDown": orbit(0, -22); break;
      case "+": case "=": dolly(0.88); break;
      case "-": dolly(1.13); break;
      default: return;
    }
    event.preventDefault();
  });
  reset.addEventListener("click", resetCamera);
  new ResizeObserver(draw).observe(canvas);

  return function show(next) {
    scene = next;
    gl.bufferData(gl.ARRAY_BUFFER, next.bytes, gl.STATIC_DRAW);
    resetCamera();
    reset.disabled = false;
  };
}

function setStatus(message = "", error = false) {
  status.hidden = !message;
  status.textContent = message;
  status.toggleAttribute("data-error", error);
}

let show;
let fileUrl;

try {
  const context = canvas.getContext("webgl", {antialias: true});
  if (context) show = makeViewer(context);
} catch (error) {
  setStatus(error.message, true);
}

async function generate() {
  const params = new URLSearchParams(new FormData(form));
  generateButton.disabled = true;
  stage.setAttribute("aria-busy", "true");
  setStatus("Generating scene…");
  try {
    const response = await fetch(`/api/scene?${params}`);
    if (!response.ok) throw new Error("Could not generate a scene. Check the preset and seed.");
    const buffer = await response.arrayBuffer();
    const next = decodeScene(buffer);
    if (show) show(next);
    const {mode, seed, objects, triangles} = next;
    const title = document.querySelector("#scene-name");
    title.textContent = mode === "race" ? "Racing circuit" : "FPS depot";
    document.querySelector("#counts").textContent =
      `${objects} meshes · ${triangles.toLocaleString()} triangles`;
    if (fileUrl) URL.revokeObjectURL(fileUrl);
    fileUrl = URL.createObjectURL(new Blob([buffer], {type: "model/gltf-binary"}));
    download.href = fileUrl;
    download.download = `georena-${mode}-${seed}.glb`;
    download.removeAttribute("aria-disabled");
    setStatus(show ? "" : "WebGL preview unavailable. The GLB is ready to download.", !show);
  } catch (error) {
    setStatus(error.message, true);
  } finally {
    stage.setAttribute("aria-busy", "false");
    generateButton.disabled = false;
  }
}

seed.addEventListener("input", () => { seed.setCustomValidity(""); });
document.querySelector("#shuffle").addEventListener("click", () => {
  seed.value = crypto.getRandomValues(new Uint32Array(1))[0];
  seed.setCustomValidity("");
});
form.addEventListener("submit", event => {
  event.preventDefault();
  if (Number(seed.value) > 4294967295) {
    seed.setCustomValidity("Enter a seed from 0 to 4294967295.");
    seed.reportValidity();
    return;
  }
  if (!generateButton.disabled) generate();
});
generate();
