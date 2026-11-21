"use strict";

function makeTopDownViewer(canvas) {
  const context = canvas.getContext("2d");
  if (!context) return null;

  let scene;
  let faces = [];

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
    context.clearRect(0, 0, pixelsWide, pixelsHigh);
    context.lineWidth = ratio * 0.8;
    const [x, , z] = scene.center;
    const scale = 0.88 * Math.min(
      canvas.width / (scene.max[0] - scene.min[0]),
      canvas.height / (scene.max[2] - scene.min[2])
    );
    const offsetX = canvas.width / 2 - x * scale;
    const offsetY = canvas.height / 2 - z * scale;

    for (const {points, color} of faces) {
      context.beginPath();
      context.moveTo(offsetX + points[0] * scale, offsetY + points[1] * scale);
      context.lineTo(offsetX + points[2] * scale, offsetY + points[3] * scale);
      context.lineTo(offsetX + points[4] * scale, offsetY + points[5] * scale);
      context.closePath();
      context.fillStyle = color;
      context.fill();
      // Cover the subpixel seams between adjacent triangles.
      context.strokeStyle = color;
      context.stroke();
    }
  }

  new ResizeObserver(draw).observe(canvas);

  return function show(next) {
    if (scene !== next) {
      scene = next;
      const {bytes, parts} = scene;
      const data = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
      faces = [];
      for (const {start, count, color: linear} of parts) {
        const color = `rgb(${linear.map(value => Math.round(Math.sqrt(value) * 255)).join(",")})`;
        for (let vertex = start; vertex < start + count; vertex += 3) {
          const offset = vertex * 24;
          // Each vertex stores a position and a normal. Skip faces that cannot be seen from above.
          if (data.getFloat32(offset + 16, true) <= 0) continue;
          const points = [];
          let height = 0;
          for (let i = 0; i < 3; i++) {
            const base = offset + i * 24;
            points.push(data.getFloat32(base, true), data.getFloat32(base + 8, true));
            height += data.getFloat32(base + 4, true);
          }
          faces.push({points, height, color});
        }
      }
      faces.sort((a, b) => a.height - b.height);
    }
    draw();
  };
}
