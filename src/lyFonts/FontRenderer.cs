/*
 *  This file is part of lysis fonts.
 *  Copyright © 2026 lysis evolutions
 *
 *  https://github.com/lyEVO/lyFonts
 */
using System;
using System.Drawing;
using Cosmos.Kernel.System.Graphics;

namespace lyFonts;

/// <summary>
/// Draws text onto a Cosmos canvas with a Font.
/// </summary>
public static class FontRenderer
{
    /// <summary>
    /// Draws a string with the given font at its default size.
    /// </summary>
    /// <param name="canvas">The canvas to draw onto.</param>
    /// <param name="text">The text to draw.</param>
    /// <param name="font">The font to draw with.</param>
    /// <param name="color">The color of the text.</param>
    /// <param name="x">The left edge of the text.</param>
    /// <param name="y">The top of the text line.</param>
    public static void DrawString(Canvas canvas, string text, Font font, Color color, int x, int y)
    {
        DrawString(canvas, text, font, font.SizePx, color, x, y);
    }

    /// <summary>
    /// Draws a string with the given font at the given size.
    /// </summary>
    /// <param name="canvas">The canvas to draw onto.</param>
    /// <param name="text">The text to draw.</param>
    /// <param name="font">The font to draw with.</param>
    /// <param name="sizePx">The text size in pixels.</param>
    /// <param name="color">The color of the text.</param>
    /// <param name="x">The left edge of the text.</param>
    /// <param name="y">The top of the text line.</param>
    public static void DrawString(Canvas canvas, string text, Font font, float sizePx, Color color, int x, int y)
    {
        ArgumentNullException.ThrowIfNull(canvas);
        ArgumentNullException.ThrowIfNull(text);
        ArgumentNullException.ThrowIfNull(font);

        int[]? buffer = canvas.GetBuffer();
        if (buffer is null)
            throw new InvalidOperationException("Canvas has no pixel buffer.");

        int width = canvas.Width;
        int height = canvas.Height;
        int baseline = y + font.GetAscent(sizePx);
        float penX = x;
        char previous = '\0';

        for (int i = 0; i < text.Length; i++)
        {
            char c = text[i];
            Glyph? glyph = font.GetGlyph(c, sizePx);
            if (glyph is null)
                continue;

            if (previous != '\0')
                penX += font.GetKerning(previous, c, sizePx);

            DrawGlyph(buffer, width, height, glyph, (int)Math.Round(penX) + glyph.OffsetX, baseline + glyph.OffsetY, color);
            penX += glyph.Advance;
            previous = c;
        }
    }

    /// <summary>
    /// Blend one glyph bitmap onto the canvas buffer.
    /// </summary>
    private static void DrawGlyph(int[] buffer, int width, int height, Glyph glyph, int glyphX, int glyphY, Color color)
    {
        byte[]? coverage = glyph.Coverage;
        if (coverage is null)
            return;

        for (int py = 0; py < glyph.Height; py++)
        {
            int destY = glyphY + py;
            if (destY < 0 || destY >= height)
                continue;

            for (int px = 0; px < glyph.Width; px++)
            {
                byte alpha = coverage[py * glyph.Width + px];
                if (alpha == 0)
                    continue;

                int destX = glyphX + px;
                if (destX < 0 || destX >= width)
                    continue;

                int index = destY * width + destX;
                Color existing = Color.FromArgb(buffer[index]);
                buffer[index] = Canvas.AlphaBlend(color, existing, alpha).ToArgb();
            }
        }
    }
}
