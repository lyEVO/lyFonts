/*
 *  This file is part of lysis fonts.
 *  Copyright © 2026 lysis evolutions
 *
 *  https://github.com/lyEVO/lyFonts
 */
using System.Runtime.InteropServices;

namespace lyFonts;

/// <summary>
/// The FreeType C functions.
/// </summary>
internal static unsafe class Native
{
    [DllImport("*")]
    internal static extern int lysis_ft_init();

    [DllImport("*")]
    internal static extern int lysis_ft_load_face(byte* data, long size);

    [DllImport("*")]
    internal static extern int lysis_ft_set_size(int face, int size26_6);

    [DllImport("*")]
    internal static extern int lysis_ft_line_height(int face);

    [DllImport("*")]
    internal static extern int lysis_ft_ascent(int face);

    [DllImport("*")]
    internal static extern int lysis_ft_descent(int face);

    [DllImport("*")]
    internal static extern int lysis_ft_family_name(int face, byte* buf, int capacity);

    [DllImport("*")]
    internal static extern int lysis_ft_style_name(int face, byte* buf, int capacity);

    [DllImport("*")]
    internal static extern int lysis_ft_render(int face, ulong charcode, byte* outBitmap, int outCapacity, int* outWidth, int* outRows, int* outLeft, int* outTop, float* outAdvance);

    [DllImport("*")]
    internal static extern float lysis_ft_kerning(int face, ulong left, ulong right);

    [DllImport("*")]
    internal static extern int lysis_ft_has_glyph(int face, ulong charcode);
}
