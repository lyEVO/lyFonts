# <img width="250" alt="lysis fonts logo" src="https://github.com/user-attachments/assets/659d434b-f12b-4de6-9f50-64c85fc80464" />
lysis fonts (lyFonts) is a port of [FreeType](https://github.com/freetype/freetype) to [Cosmos](https://github.com/CosmosOS/Cosmos) and can be easily used within Cosmos-based operating systems.

## What it is

lyFonts brings FreeType to Cosmos. It renders TTF and OTF fonts.

`Face` is the low level part that interfaces with FreeType. You can give it font bytes and it will render glyphs to bitmaps with metrics. Custom text layouts can be built on top of it.

`Font` and `FontRenderer` are the high level parts. `Font` is a font with a size and metrics. `FontRenderer.DrawString` takes a Canvas (from Cosmos Graphics Subsystem) and draws text onto it.

## How to use it

Add the package:

```bash
dotnet add package lyFonts
```

Add a font to your project as an embedded resource, then load it and draw.

```csharp
using System.Reflection;
using Cosmos.Kernel.System.Graphics;
using lyFonts;

// Load the font bytes from an embedded resource.
using Stream? stream = Assembly.GetExecutingAssembly()
    .GetManifestResourceStream("Kernel.Font.ttf");
using MemoryStream memory = new MemoryStream();
stream.CopyTo(memory);
byte[] fontData = memory.ToArray();

// A font at a size.
lyFonts.Font font = new lyFonts.Font(fontData, 20);

// Draw it on a canvas.
Canvas canvas = Canvas.GetFullScreen();
FontRenderer.DrawString(canvas, "Hello, world!", font, 20, Color.White, 40, 40);
canvas.Display();
```

## Credits

- Drawing structure inspired by [CosmosTTF](https://github.com/retrievaaaa/CosmosTTF).
- TrueType font drawing follows [Cosmos's TrueTypeFont](https://github.com/CosmosOS/Cosmos/tree/gen3/src/Cosmos.Kernel.System/Graphics/Fonts).
- FreeType, under the FTL license. lyFonts's own code is under the 3-Clause BSD License, see [LICENSE](LICENSE).
