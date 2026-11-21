(() => {
  "use strict";
  const packs = {
    arena: ["arena-surfaces.jpg", "Asphalt", "Beach sand", "Alpine grass", "Slate"],
    props: ["prop-materials.jpg", "Crate wood", "Container steel", "Tire rubber", "Concrete"],
    decals: ["racing-decals.png", "Racing stripes", "Chevrons", "Number roundel", "Finish checkers"],
    weather: ["weather.png", "Rain", "Snow", "Wind", "Fog"],
    elements: ["elements.png", "Fire", "Water", "Ice", "Lightning"],
  };

  const palette = document.querySelector("#asset-palette");
  const collection = palette.querySelector("#asset-collection");
  const grid = palette.querySelector("#asset-grid");
  const light = palette.querySelector("#asset-light");
  const download = palette.querySelector("#asset-download");
  const status = palette.querySelector("#asset-status");
  const tiles = Array.from({length: 4}, () => {
    const figure = document.createElement("figure");
    figure.innerHTML = '<div class="asset-swatch"><img alt=""></div><figcaption></figcaption>';
    grid.append(figure);
    return {image: figure.querySelector("img"), label: figure.querySelector("figcaption")};
  });

  let active = 0;
  async function showCollection() {
    const request = ++active;
    const [file, ...labels] = packs[collection.value];
    const url = "/assets/ui/" + file;
    light.closest("label").hidden = !file.endsWith(".png");
    download.href = url;
    grid.setAttribute("aria-busy", "true");
    status.textContent = "Loading sheet…";
    status.hidden = false;
    delete status.dataset.error;
    for (const [i, tile] of tiles.entries()) {
      tile.label.textContent = labels[i];
      tile.image.src = url;
    }

    let loaded = false;
    try {
      await Promise.all(tiles.map(tile => tile.image.decode()));
      loaded = true;
    } catch {
      // The status below preserves the labels when a sheet fails to load.
    }
    if (request !== active) return;
    grid.setAttribute("aria-busy", "false");
    status.hidden = loaded;
    status.toggleAttribute("data-error", !loaded);
    status.textContent = loaded ? "" : "This sheet could not be loaded.";
  }

  palette.addEventListener("toggle", () => {
    if (palette.open && active === 0) showCollection();
  });
  collection.addEventListener("change", showCollection);
  light.addEventListener("change", () => {
    grid.classList.toggle("asset-grid-light", light.checked);
  });
})();
