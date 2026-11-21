(() => {
  "use strict";
  const packs = {
    arena: ["arena-surfaces.jpg", "Asphalt", "Beach sand", "Alpine grass", "Slate"],
    props: ["prop-materials.jpg", "Crate wood", "Container steel", "Tire rubber", "Concrete"],
    decals: ["racing-decals.png", "Racing stripes", "Chevrons", "Number roundel", "Finish checkers"],
    weather: ["weather.png", "Rain", "Snow", "Wind", "Fog"],
    elements: ["elements.png", "Fire", "Water", "Ice", "Lightning"],
    racingAvatars: ["racing-avatars.png", "Orange helmet", "Teal rally helmet", "Ivory helmet", "Yellow helmet"],
    hud: ["hud-icons.png", "Stopwatch", "Finish flag", "Trophy", "Shield"],
    fpsAvatars: ["fps-avatars.png", "Amber visor", "Cyan visor", "Amber open helmet", "Cyan open helmet"],
    fpsHud: ["fps-hud-icons.png", "Health", "Charge", "Objective", "Reticle"],
    fpsObjectives: ["fps-objective-badges.png", "Flag Capture", "Team Assist", "Hold the Line", "Comeback"],
    fpsPerformance: ["fps-performance-badges.png", "First Victory", "Win Streak", "Supply Runner", "Overtime Clutch"],
    trophies: ["reward-trophies.png", "Bronze", "Silver", "Gold", "Champion"],
    racingPowerups: ["racing-powerups.png", "Extra Grip", "Pickup Magnet", "Score Multiplier", "Ghost Pass"],
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
