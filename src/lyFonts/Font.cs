/*
 *  This file is part of lysis fonts.
 *  Copyright © 2026 lysis evolutions
 *
 *  https://github.com/lyEVO/lyFonts
 */
using System;
using System.IO;

namespace lyFonts;

/// <summary>
/// A font, backed by a FreeType face.
/// Holds metrics and glyphs.
/// </summary>
public class Font : IDisposable
{
    /// <summary>
    /// Wrap an existing face.
    /// </summary>
    /// <param name="face">The FreeType face to read glyphs from.</param>
    /// <param name="sizePx">The default text size in pixels.</param>
    public Font(Face face, int sizePx = 16)
    {
        Face = face;
        SizePx = sizePx;
    }

    /// <summary>
    /// Load a font from the raw bytes of a TTF or OTF file.
    /// </summary>
    /// <param name="fontData">The raw bytes of the font file.</param>
    /// <param name="sizePx">The default text size in pixels.</param>
    public Font(byte[] fontData, int sizePx = 16)
        : this(new Face(fontData), sizePx)
    {
        _ownsFace = true;
    }

    /// <summary>
    /// Load a font from a file.
    /// </summary>
    /// <param name="path">The path of the font file.</param>
    /// <param name="sizePx">The default text size in pixels.</param>
    public Font(string path, int sizePx = 16)
        : this(File.ReadAllBytes(path), sizePx)
    {
    }

    /// <summary>
    /// The FreeType face behind this font.
    /// </summary>
    public Face Face { get; }

    /// <summary>
    /// The text size in pixels used when no explicit size is given.
    /// </summary>
    public int SizePx { get; set; }

    /// <summary>
    /// Get the family name of the font.
    /// </summary>
    public string GetFamilyName() => Face.GetFamilyName();

    /// <summary>
    /// Get the style name of the font, for example Regular or Bold.
    /// </summary>
    public string GetStyleName() => Face.GetStyleName();

    /// <summary>
    /// Returns whether the font contains a glyph for the given character.
    /// </summary>
    public bool HasGlyph(char c)
    {
        return Face.HasGlyph(c);
    }

    /// <summary>
    /// Gets the glyph for a character at this font's size.
    /// </summary>
    public Glyph? GetGlyph(char c)
    {
        return GetGlyph(c, SizePx);
    }

    /// <summary>
    /// Gets the glyph for a character at the given size.
    /// </summary>
    public Glyph? GetGlyph(char c, int sizePx)
    {
        return Face.GetGlyph(c, sizePx);
    }

    /// <summary>
    /// Gets the distance from the top of a text line
    /// to the baseline at this font's size.
    /// </summary>
    public int GetAscent()
    {
        return GetAscent(SizePx);
    }

    /// <summary>
    /// Gets the distance from the top of a text line
    /// to the baseline at the given size.
    /// </summary>
    public int GetAscent(int sizePx)
    {
        return Face.GetAscent(sizePx);
    }

    /// <summary>
    /// Gets the distance between the tops of two
    /// consecutive text lines at this font's size.
    /// </summary>
    public int GetLineHeight()
    {
        return GetLineHeight(SizePx);
    }

    /// <summary>
    /// Gets the distance between the tops of two
    /// consecutive text lines at the given size.
    /// </summary>
    public int GetLineHeight(int sizePx)
    {
        return Face.GetLineHeight(sizePx);
    }

    /// <summary>
    /// Gets the vertical metrics of the font
    /// at this font's size.
    /// </summary>
    /// <param name="ascent">The distance from the top of a text line to the baseline.</param>
    /// <param name="descent">The distance from the baseline to the bottom of the line. Negative.</param>
    /// <param name="lineGap">The extra spacing between two lines.</param>
    public void GetLineMetrics(out int ascent, out int descent, out int lineGap)
    {
        GetLineMetrics(SizePx, out ascent, out descent, out lineGap);
    }

    /// <summary>
    /// Gets the vertical metrics of the font at the given size.
    /// </summary>
    /// <param name="sizePx">The text size in pixels.</param>
    /// <param name="ascent">The distance from the top of a text line to the baseline.</param>
    /// <param name="descent">The distance from the baseline to the bottom of the line. Negative.</param>
    /// <param name="lineGap">The extra spacing between two lines.</param>
    public void GetLineMetrics(int sizePx, out int ascent, out int descent, out int lineGap)
    {
        ascent = Face.GetAscent(sizePx);
        descent = Face.GetDescent(sizePx);
        lineGap = Face.GetLineHeight(sizePx) - (ascent - descent);
    }

    /// <summary>
    /// Gets the horizontal advance of a single character
    /// at this font's size, without kerning.
    /// Zero when the font has no glyph for the character.
    /// </summary>
    public int GetAdvance(char c)
    {
        return GetAdvance(c, SizePx);
    }

    /// <summary>
    /// Gets the horizontal advance of a single character
    /// at the given size, without kerning.
    /// Zero when the font has no glyph for the character.
    /// </summary>
    public int GetAdvance(char c, int sizePx)
    {
        return Face.GetGlyph(c, sizePx)?.Advance ?? 0;
    }

    /// <summary>
    /// Gets the kerning adjustment for a pair
    /// of characters at this font's size.
    /// </summary>
    public int GetKerning(char left, char right)
    {
        return GetKerning(left, right, SizePx);
    }

    /// <summary>
    /// Gets the kerning adjustment for a pair
    /// of characters at the given size.
    /// </summary>
    public int GetKerning(char left, char right, int sizePx)
    {
        return Face.GetKerning(left, right, sizePx);
    }

    /// <summary>
    /// Gets the widest advance any printable character needs.
    /// </summary>
    public int GetMaxAdvance()
    {
        int maxAdvance = 0;

        for (char c = '!'; c <= '~'; c++)
        {
            int advance = GetAdvance(c, SizePx);
            if (advance > maxAdvance)
            {
                maxAdvance = advance;
            }
        }

        return maxAdvance;
    }

    /// <summary>
    /// Measures the width that drawing the given text
    /// at this font's size would use.
    /// </summary>
    public int MeasureString(string text)
    {
        return MeasureString(text, SizePx);
    }

    /// <summary>
    /// Measures the width that drawing the given text
    /// at the given size would use.
    /// </summary>
    public int MeasureString(string text, int sizePx)
    {
        int width = 0;
        char previous = '\0';

        for (int i = 0; i < text.Length; i++)
        {
            char c = text[i];
            Glyph? glyph = GetGlyph(c, sizePx);
            if (glyph is null)
            {
                continue;
            }

            if (previous != '\0')
            {
                width += GetKerning(previous, c, sizePx);
            }

            width += glyph.Advance;
            previous = c;
        }

        return width;
    }

    /// <summary>
    /// Release the face, but only when this font loaded it itself.
    /// </summary>
    public void Dispose()
    {
        if (_ownsFace)
            Face.Dispose();
        GC.SuppressFinalize(this);
    }

    /// <summary>
    /// True when this font loaded its own face.
    /// </summary>
    private readonly bool _ownsFace;
}
