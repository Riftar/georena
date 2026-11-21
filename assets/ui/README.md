# UI asset sheets

These bundled sheets were made with built-in ImageGen. The Workbench includes an **Asset palette** for previewing and downloading them offline.

| Sheet | Format | Top left | Top right | Bottom left | Bottom right |
| --- | --- | --- | --- | --- | --- |
| [Arena surfaces](arena-surfaces.jpg) | 1024 × 1024 JPEG | Asphalt | Beach sand | Alpine grass | Slate |
| [Prop materials](prop-materials.jpg) | 1024 × 1024 JPEG | Crate wood | Container steel | Tire rubber | Concrete |
| [Racing decals](racing-decals.png) | 768 × 768 PNG with alpha | Racing stripes | Chevrons | 07 roundel | Finish checkers |
| [Weather](weather.png) | 768 × 768 PNG with alpha | Rain | Snow | Wind | Fog |
| [Elements](elements.png) | 768 × 768 PNG with alpha | Fire | Water | Ice | Lightning |
| [Racing avatars](racing-avatars.png) | 768 × 768 PNG with alpha | Orange helmet | Teal rally helmet | Ivory helmet | Yellow helmet |
| [Shared HUD icons](hud-icons.png) | 768 × 768 PNG with alpha | Stopwatch | Finish flag | Trophy | Shield |
| [FPS avatars](fps-avatars.png) | 768 × 768 PNG with alpha | Amber visor | Cyan visor | Amber open helmet | Cyan open helmet |
| [FPS HUD icons](fps-hud-icons.png) | 768 × 768 PNG with alpha | Health | Charge | Objective | Reticle |

Grid cells start at 0% or 50% of the image on each axis. Each portrait or HUD icon occupies one 384 × 384 cell. The surface sheets are swatches for UI previews. The decals and effect sprites are static images. GeoRena's GLB exports currently use flat material colors.

<details>
<summary>Image generation prompts</summary>

**Arena surfaces**

> Generate a square texture atlas for the GeoRena low-poly game editor UI. Exactly four evenly sized material swatches in a clean 2 by 2 grid, divided at the exact horizontal and vertical midpoints, edge to edge with zero padding, no lines and no labels. Top left is charcoal asphalt with fine gray aggregate. Top right is warm pale beach sand with shallow wind ripples. Bottom left is muted alpine grass made of a few broad overlapping low-poly facets. Bottom right is cool gray exposed rock with angular slate facets. All four are flat orthographic surface samples at matching scale, matte, softly colored, and uniformly lit. Fill each quadrant completely with its own material. No objects, perspective, horizon, cast shadows, text, logos, or watermark.

**Prop materials**

> Generate a square texture atlas for generic prop materials in the GeoRena low-poly game editor UI. Exactly four evenly sized material swatches in a clean 2 by 2 grid, divided at the exact horizontal and vertical midpoints, edge to edge with zero padding, no lines and no labels. Top left is warm rough wooden crate planks with subtle two-tone grain. Top right is muted blue painted corrugated shipping-container steel with a few small scratches. Bottom left is dark charcoal vehicle tire rubber showing a simple diagonal tread pattern. Bottom right is pale warm gray cast concrete with restrained small pores. Flat orthographic surface samples, matte, simplified game texture style, restrained contrast, and uniformly lit. Fill each quadrant completely with its own material. No objects, perspective, horizon, cast shadows, text, logos, or watermark.

**Racing decals**

> Generate a square transparent racing decal atlas for original low-poly game cars in GeoRena. Put exactly four separate flat print-ready vinyl designs in an evenly spaced 2 by 2 grid, each centered inside its quadrant with generous transparent padding on all sides. Top left: two long parallel ivory racing stripes, one thick and one thin. Top right: two bold amber forward chevrons, simple blunt corners. Bottom left: an ivory circular number roundel containing the exact dark charcoal number '07'. Bottom right: a short two-row ivory and charcoal checkered finish stripe with six columns. Use crisp hard edges and solid fills. Transparent background between and around all designs, including the gaps within each design. No shadows, reflections, texture, vehicle mockup, grid lines, borders, extra labels, branding, or watermark. The only text is the 07 inside the roundel.

**Weather**

> Create a square transparent weather sprite sheet for GeoRena's low-poly game editor. Four independent static weather effect graphics in an exact 2 by 2 grid, each centered within its quadrant and fully contained with at least 15 percent clear transparent padding. Top left: a cluster of distinct blue slanted rain streaks. Top right: six drifting ivory and pale blue snowflakes in varied sizes. Bottom left: three curved mint-blue gust lines sweeping to the right. Bottom right: pale steel-blue horizontal fog wisps, layered with gentle alpha falloff, readable as fog on both dark and light UI panels. Match simple hand-painted faceted game art, clear silhouettes, high legibility at thumbnail size. True alpha background throughout and between all particles. No backgrounds, cloud icons, outlines around the cells, text, grid lines, scene elements, watermarks, or extra graphics.

**Elements**

> Create a square transparent elemental effect sprite sheet for GeoRena's low-poly game editor. Four independent, clearly legible static effect sprites in an exact 2 by 2 grid, each centered inside its own quadrant and fully contained with at least 15 percent clear transparent padding. Top left: an angular orange and scarlet fire flame with golden center. Top right: a crisp cyan water splash with a few suspended droplets. Bottom left: an icy blue cluster of sharp translucent frost crystals. Bottom right: a golden-white jagged lightning bolt with two fine violet electrical branches. Faceted hand-painted low-poly game art, clean silhouettes, balanced visual scale, designed to read at thumbnail size. True alpha background around each effect and between particles. No ground plane, pedestal, scene, sky, texture fills outside the sprites, text, cell borders, logos, or watermarks.

+**Racing avatars**

> Create a square 2 by 2 atlas of four original low-poly player avatar portraits for GeoRena's in-game player badges and scoreboards. Real transparent alpha between and around the portraits. Place each equal-sized head-and-shoulders silhouette dead center inside its own quadrant with generous outer padding, consistent eye level, identical scale, and no overlapping cells. Top left: a coral orange full-face competition helmet with a dark inset visor. Top right: a teal rally helmet with squared goggles and a short peaked brim. Bottom left: an ivory open-face helmet and friendly dark-skinned adult woman with a clear, readable face. Bottom right: a mustard-yellow open-face helmet and adult man with medium brown skin, subtle stubble and a slate collar. Original fictional characters, bright distinct color blocks, simple angular facets, readable in a 48-pixel square. Faces shown in consistent three-quarter front view. No tiles behind portraits, circles, boundaries, text, numbers, emblems, logos, game UI frame, or watermark.

**Shared HUD icons**

> Create a square transparent 2 by 2 atlas of four clean HUD icons for the low-poly racing and arena prototype GeoRena. Exactly one separate symbol per quadrant, centered, consistent size and weight, with at least 15 percent of each quadrant clear on all sides. Top left: a stopwatch with one bold hand. Top right: an ivory-and-charcoal checkered flag on a short upright pole. Bottom left: an amber two-handle trophy cup. Bottom right: a mint-cyan shield. Sharp silhouettes and simple angular geometry, very restrained low-poly facets, few solid fills, dark edge accents so icons read at 24 pixels on both light and dark backdrops. True alpha background around every icon and in gaps where appropriate. No cards, circles behind the icons, shadows, text, labels, digits, decorations, extra icons, visible grid or watermark.

**FPS avatars**

> Create a square transparent 2 by 2 atlas of four original low-poly player portraits for a fictional competition in GeoRena's industrial depot. These are stylized first-person arena competitors in invented padded sports gear, no real uniforms, no equipment in hands. Equal scale and eye height, all head-and-shoulders, each centered in its quadrant with at least 12 percent transparent padding and no overlap. Top left: an amber-team competitor in an amber angular closed helmet with a smoked visor and charcoal collar. Top right: a cyan-team competitor in a vivid teal rounded segmented helmet with a teal mirrored visor and slate collar. Bottom left: a dark-skinned adult woman in an open amber helmet and ear guards. Bottom right: an adult man in an open teal helmet and gray goggles pushed above his eyes. Original, faceted low-poly game art, clean silhouettes readable at 48 pixels. True alpha around portraits, no frame, grid lines, text, weapons, real affiliations, branding, or watermark.

**FPS HUD icons**

> Create a square transparent 2 by 2 atlas of four clean FPS game HUD icons for GeoRena's fictional depot arena. Exactly one separate symbol per quadrant, centered and evenly sized, with generous transparent margins. Top left: a faceted coral red heart representing health. Top right: a cyan energy cell with a single white charge indicator. Bottom left: an amber diamond-shaped objective marker with a pale central dot. Bottom right: a restrained ivory four-tick reticle with an empty center. Original stylized low-poly forms, sharp silhouettes, low detail, high legibility at 24 pixels. True transparent alpha between and around all marks. No cards, backgrounds, shadows, additional items, weapon silhouettes, text, digits, logos, or watermark.

</details>
