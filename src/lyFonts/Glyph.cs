/*
 *  This file is part of lysis fonts.
 *  Copyright © 2026 lysis evolutions
 *
 *  https://github.com/lyEVO/lyFonts
 */
namespace lyFonts;

/// <summary>
/// A rasterized glyph.
/// </summary>
public class Glyph
{
    /// <summary>
    /// Make a glyph from a render.
    /// </summary>
    public Glyph(byte[]? coverage, int width, int height, int offsetX, int offsetY, int advance)
    {
        Coverage = coverage;
        Width = width;
        Height = height;
        OffsetX = offsetX;
        OffsetY = offsetY;
        Advance = advance;
    }

    /// <summary>
    /// One coverage byte per pixel, row-major.
    /// Null for whitespace that only moves the pen.
    /// </summary>
    public byte[]? Coverage { get; }

    /// <summary>
    /// The width of the coverage bitmap in pixels.
    /// </summary>
    public int Width { get; }

    /// <summary>
    /// The height of the coverage bitmap in pixels.
    /// </summary>
    public int Height { get; }

    /// <summary>
    /// The offset from the pen to the left edge of the bitmap.
    /// </summary>
    public int OffsetX { get; }

    /// <summary>
    /// The offset from the baseline to the top edge of the bitmap.
    /// Negative above the baseline.
    /// </summary>
    public int OffsetY { get; }

    /// <summary>
    /// How far the pen moves right after this glyph.
    /// </summary>
    public int Advance { get; }
}
