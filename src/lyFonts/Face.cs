/*
 *  This file is part of lysis fonts.
 *  Copyright © 2026 lysis evolutions
 *
 *  https://github.com/lyEVO/lyFonts
 */
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace lyFonts;

/// <summary>
/// A face loaded from a font file.
/// </summary>
public unsafe class Face : IDisposable
{
    /// <summary>
    /// Load a face from the bytes of a font file.
    /// </summary>
    /// <param name="fontData">The font file bytes.</param>
    public Face(byte[] fontData)
    {
        _bytes = fontData;

        if (Native.lysis_ft_init() != 0)
            throw new InvalidOperationException("FreeType init failed.");

        // pin the bytes as FreeType reads them directly
        _pin = GCHandle.Alloc(_bytes, GCHandleType.Pinned);

        byte* bytes = (byte*)_pin.AddrOfPinnedObject();
        _handle = Native.lysis_ft_load_face(bytes, _bytes.Length);
        if (_handle < 0)
        {
            _pin.Free();
            throw new InvalidDataException("Not a valid font.");
        }
    }

    /// <summary>
    /// Get the family name.
    /// </summary>
    public string GetFamilyName()
    {
        if (_family is null)
            _family = ReadName(true);
        return _family;
    }

    /// <summary>
    /// Get the style name.
    /// </summary>
    public string GetStyleName()
    {
        if (_style is null)
            _style = ReadName(false);
        return _style;
    }

    /// <summary>
    /// Get the line height at a size.
    /// </summary>
    public int GetLineHeight(int pixelSize)
    {
        if (!_heights.TryGetValue(pixelSize, out int height))
        {
            SetSize(pixelSize);
            height = Native.lysis_ft_line_height(_handle);
            _heights[pixelSize] = height;
        }
        return height;
    }

    /// <summary>
    /// Get the ascent at a size.
    /// Distance from the top of the line to the baseline.
    /// </summary>
    public int GetAscent(int pixelSize)
    {
        if (!_ascents.TryGetValue(pixelSize, out int ascent))
        {
            SetSize(pixelSize);
            ascent = Native.lysis_ft_ascent(_handle);
            _ascents[pixelSize] = ascent;
        }
        return ascent;
    }

    /// <summary>
    /// Get the descent at a size.
    /// Distance from the baseline to the bottom of the line.
    /// </summary>
    public int GetDescent(int pixelSize)
    {
        if (!_descents.TryGetValue(pixelSize, out int descent))
        {
            SetSize(pixelSize);
            descent = Native.lysis_ft_descent(_handle);
            _descents[pixelSize] = descent;
        }
        return descent;
    }

    /// <summary>
    /// Get the kerning for two characters at a size.
    /// </summary>
    public int GetKerning(char left, char right, int pixelSize)
    {
        SetSize(pixelSize);
        int kerning = Native.lysis_ft_kerning(_handle, left, right);
        return kerning;
    }

    /// <summary>
    /// Check if the font has a glyph for a character.
    /// </summary>
    public bool HasGlyph(char c)
    {
        int found = Native.lysis_ft_has_glyph(_handle, c);
        return found != 0;
    }

    /// <summary>
    /// Get the glyph for a character at a size.
    /// </summary>
    public Glyph? GetGlyph(char c, int pixelSize)
    {
        if (pixelSize is <= 0 or > ushort.MaxValue)
            throw new ArgumentOutOfRangeException(nameof(pixelSize), $"Text size ({pixelSize}) must be between 1 and {ushort.MaxValue}");

        int key = (pixelSize << 16) | c;
        if (!_glyphs.TryGetValue(key, out Glyph? glyph))
        {
            glyph = Rasterize(c, pixelSize);
            _glyphs[key] = glyph;
        }
        return glyph;
    }

    /// <summary>
    /// Render a single glyph.
    /// </summary>
    private Glyph? Rasterize(char c, int pixelSize)
    {
        SetSize(pixelSize);

        int glyphWidth;
        int glyphRows;
        int glyphLeft;
        int glyphTop;
        int glyphAdvance;

        // first call measures the glyph, and the buffer comes after
        int result = Native.lysis_ft_render(_handle, c, null, 0, &glyphWidth, &glyphRows, &glyphLeft, &glyphTop, &glyphAdvance);
        if (result == -1)
            return null;

        // a glyph with no bitmap only moves the pen
        if (glyphWidth == 0 || glyphRows == 0)
            return new Glyph(null, 0, 0, glyphLeft, -glyphTop, glyphAdvance);

        byte[] bitmap = new byte[glyphWidth * glyphRows];
        fixed (byte* pixels = bitmap)
        {
            result = Native.lysis_ft_render(_handle, c, pixels, bitmap.Length, &glyphWidth, &glyphRows, &glyphLeft, &glyphTop, &glyphAdvance);
            if (result != 0)
                return null;
        }

        return new Glyph(bitmap, glyphWidth, glyphRows, glyphLeft, -glyphTop, glyphAdvance);
    }

    /// <summary>
    /// Set the size, but only when it is different.
    /// </summary>
    private void SetSize(int pixelSize)
    {
        if (pixelSize == _size)
            return;

        if (Native.lysis_ft_set_size(_handle, pixelSize) != 0)
            throw new InvalidOperationException("FreeType set size failed.");

        _size = pixelSize;
    }

    /// <summary>
    /// Read a family or style name.
    /// </summary>
    private string ReadName(bool family)
    {
        byte* buffer = stackalloc byte[128];
        int result;
        if (family)
            result = Native.lysis_ft_family_name(_handle, buffer, 128);
        else
            result = Native.lysis_ft_style_name(_handle, buffer, 128);

        if (result != 0)
            return string.Empty;

        int length = 0;
        for (; length < 128; length++)
        {
            if (buffer[length] == 0)
                break;
        }
        return Encoding.UTF8.GetString(new ReadOnlySpan<byte>(buffer, length));
    }

    /// <summary>
    /// Free the pin on the font bytes.
    /// </summary>
    public void Dispose()
    {
        if (_pin.IsAllocated)
            _pin.Free();
        GC.SuppressFinalize(this);
    }

    /// <summary>
    /// Raw font file bytes.
    /// </summary>
    private readonly byte[] _bytes;

    /// <summary>
    /// FreeType face handle.
    /// </summary>
    private readonly int _handle;

    /// <summary>
    /// Keeps the bytes pinned for FreeType.
    /// </summary>
    private GCHandle _pin;

    /// <summary>
    /// Current face size.
    /// </summary>
    private int? _size;

    /// <summary>
    /// Rendered glyphs, by size and character.
    /// </summary>
    private readonly Dictionary<int, Glyph?> _glyphs = new();

    /// <summary>
    /// Line heights by size.
    /// </summary>
    private readonly Dictionary<int, int> _heights = new();

    /// <summary>
    /// Ascents by size.
    /// </summary>
    private readonly Dictionary<int, int> _ascents = new();

    /// <summary>
    /// Descents by size.
    /// </summary>
    private readonly Dictionary<int, int> _descents = new();

    /// <summary>
    /// Cached family name.
    /// </summary>
    private string? _family;

    /// <summary>
    /// Cached style name.
    /// </summary>
    private string? _style;
}
