# UI asset sheets

These bundled sheets were made with built-in ImageGen. The Workbench includes an **Asset palette** for previewing and downloading them offline.

The artwork is included under the [MIT License](../../LICENSE).

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
| [FPS objective badges](fps-objective-badges.png) | 768 × 768 PNG with alpha | Flag Capture | Team Assist | Hold the Line | Comeback |
| [FPS performance badges](fps-performance-badges.png) | 768 × 768 PNG with alpha | First Victory | Win Streak | Supply Runner | Overtime Clutch |
| [Reward trophies](reward-trophies.png) | 768 × 768 PNG with alpha | Bronze | Silver | Gold | Champion |
| [Racing power-ups](racing-powerups.png) | 768 × 768 PNG with alpha | Extra Grip | Pickup Magnet | Score Multiplier | Ghost Pass |

Grid cells start at 0% or 50% of the image on each axis. Every 768 × 768 sheet contains four 384 × 384 cells. The surface sheets are swatches for UI previews. The decals and effect sprites are static images. GeoRena's GLB exports currently use flat material colors.

## Reuse in game UI

| Use | Existing art or display data |
| --- | --- |
| Racing boost | Lightning from [Elements](elements.png) |
| Racing protection or time bonus | Shield or Stopwatch from [Shared HUD icons](hud-icons.png) |
| Slippery track hazard | Water or Ice from [Elements](elements.png) |
| Repair or recharge | Health or Charge from [FPS HUD icons](fps-hud-icons.png) |
| Racing checkpoint or shared objective | Objective diamond from [FPS HUD icons](fps-hud-icons.png) |
| Lap completion and match results | Finish flag and Trophy for compact indicators. Use [Reward trophies](reward-trophies.png) for award cards. |
| Player badges and scoreboards | Use [racing portraits](racing-avatars.png) or [FPS team portraits](fps-avatars.png) alongside live player names, ranks, scores, or lap times. |
| Minimap, countdown, and progress | Draw these from arena geometry and live game state. |

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

+**FPS objective badges**

> Create a square 2 by 2 atlas of four achievement badges for GeoRena's fictional first-person competition in an industrial depot. The four original badges are isolated on real transparency and centered one per quadrant. Use the same compact beveled dark slate medallion silhouette with different large symbols and readable amber or cyan accent colors. Top left: an amber planted flag over a diamond checkpoint, for Flag Capture. Top right: two interlocking cyan links, for Team Assist. Bottom left: a cyan shield in front of a short reinforced barrier, for Hold the Line. Bottom right: a bold ivory curved arrow recovering upward around an amber center, for Comeback. Low-poly 3D game UI emblem art, sharp silhouettes, very few large facets, soft studio illumination, fully readable at 48 pixels. The decoration and badge must be contained completely within their cells, with at least 10 percent clean transparent padding. No card backgrounds, ground, cast shadows, motion effects, weapons, text, labels, letters, numerals, logos, or watermark.

**FPS performance badges**

> Create a square 2 by 2 atlas of four original performance achievement badges for GeoRena's fictional industrial depot arena. Exactly one isolated badge per quadrant, centered in a regular grid with generous transparent padding. Use a repeated compact beveled dark slate medallion silhouette with angular game-art facets and large icon shapes. Top left: a single ivory star above a small amber winner's podium, for First Victory. Top right: three ascending gold chevrons, for Win Streak. Bottom left: a teal supply crate with a broad amber diagonal stripe and a small forward arrow, for Supply Runner. Bottom right: a steel-blue stopwatch with an amber wedge and one strong hand, for Overtime Clutch. Stylized low-poly 3D UI rewards with consistent icon scale and studio lighting, readable at thumbnail size. True alpha background all around each badge. No grounding shadow, text, labels, letters, numbers, weapon shapes, logos, watermark, or grid lines.

**Reward trophies**

> Create a square transparent 2 by 2 trophy atlas for GeoRena, a low-poly competitive racing and first-person arena prototype. Exactly four original award trophies, isolated and each centered within its own quadrant, similar visual scale with entire base visible and ample clear padding. Top left: a squat warm bronze two-handle cup on a compact dark stone base. Top right: a clean silver chalice with hooked handles and a cool teal inset in the base. Bottom left: a bright gold angular cup with tapered handles and a broad dark base. Bottom right: a champion trophy, a dark obsidian prism topped by a faceted gold star and framed by cyan enamel fins, taller and more distinctive than the other awards. Front-facing three-quarter angle and consistent camera, clearly readable silhouette at 64 pixels, deliberate low-poly facets and restrained metallic highlights. True alpha behind every object, including between handles. No floor, glow, cast shadows, sparkles, text, base engravings, labels, logos, real-world trophies, or watermark.

**Trophy atlas spacing edit**

> Edit this GeoRena trophy atlas while preserving its four original trophy designs and true transparency. Keep the bronze cup top-left, silver cup top-right, gold cup bottom-left, and the obsidian star champion trophy bottom-right. Make a precise mathematical 2-by-2 sprite sheet. Crucially, shrink each whole trophy so it occupies at most 72% of both the height and width of its individual quadrant. Center each complete trophy precisely at its quadrant's center (25%,25%; 75%,25%; 25%,75%; 75%,75%). The champion star is ENTIRELY below the horizontal 50% seam, with a wide transparent gap above it. There must be a broad uninterrupted transparent gutter across the exact horizontal and vertical middle of the square, at least 12% of the full sheet width/height. All four complete objects are isolated with visible full bases and stars. Preserve the original bronze, silver, gold, and cyan-obsidian materials, faceted low-poly styling, and proportions within each trophy. True alpha beyond the objects and in handle holes. Do not add text, dividers, backgrounds, shadows, decoration, objects, or logos.

**Racing power-ups**

> Create a square transparent 2-by-2 atlas of four original racing pickup icons for the GeoRena low-poly game. Each fully visible icon is centered within its own exact quadrant and occupies between 60 and 74 percent of the quadrant, leaving a wide transparent horizontal and vertical gutter across the exact midlines. Top left: a charcoal tire with chunky zigzag tread and a small vivid green traction accent, for Extra Grip. Top right: an amber and steel horseshoe magnet with cyan pole tips, for Pickup Magnet. Bottom left: two chunky staggered gold score tokens, the front token bearing one ivory star, for a Score Multiplier. Bottom right: a ghostly translucent pale cyan sports-car silhouette passing through a small split geometric gate, for a Ghost Pass, readable even at thumbnail size. Match a cohesive original low-poly game UI kit with faceted forms, simplified silhouettes, clean materials and restrained highlights. True alpha outside all the icons. No extra objects, visible grid, card frames, text, digits, logos, shadows on an imaginary floor, or watermark.

</details>
