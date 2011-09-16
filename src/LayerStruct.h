#ifndef LAYERSTRUCT_H
#define LAYERSTRUCT_H


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>

#include <gtk/gtk.h>
#include <cairo.h>
#include <cairo-svg.h>
#include <cairo-pdf.h>
#include <cairo-ps.h>
#include <cairo-ft.h>

#include "Layer.h"


class LayerStruct;
typedef void(LayerStruct::*LSMemFunPtr)(void *, gboolean);

class LayerStruct
{
public:
    LayerStruct(const gchar* lName, gint id, gdouble wInPoints, gdouble hInPoints, gint wInPixels, gint hInPixels);
    ~LayerStruct();

    // Layer operations
    inline void SetLayerName(const gchar* lName) { g_string_assign(layerName, lName); }

    // Layer Information
    inline const gchar* GetLayerName() { return layerName->str; }
    inline gint GetLayerId() { return ID; }

    // Layer transformation
    void SetDefaultTransMatrix();
    inline cairo_matrix_t GetTransMatrix() { return transMat; }
    // Scale
    inline gdouble GetScaleCenterX() { return scaleCenterX; }
    inline gdouble GetScaleCenterY() { return scaleCenterY; }
    inline void SetScaleCenter(gdouble scCenterX, gdouble scCenterY) { scaleCenterX = scCenterX; scaleCenterY = scCenterY; }
    inline void SetScale(gdouble scX, gdouble scY) { scaleX = scX; scaleY = scY; }
    void Scale(gdouble scX, gdouble scY);
    // Rotate
    inline gdouble GetRotateCenterX() { return rotCenterX; }
    inline gdouble GetRotateCenterY() { return rotCenterY; }
    inline void SetRotateCenter(gdouble rCenterX, gdouble rCenterY) { rotCenterX = rCenterX; rotCenterY = rCenterY; }
    inline void SetRotate(gdouble rAngle) { rotAngle = rAngle; }
    void Rotate(gdouble rAngle);
    void Rotate(gdouble cosAngle, gdouble sinAngle);
    // Move
    void SetMove(gdouble trX, gdouble trY) { transX = trX; transY = trY; }
    void Move(gdouble trX, gdouble trY);

    // Context operations
    void Clear();
    void PaintWithBgColor(pGEcontext gc, gboolean allowZeroAlpha = TRUE);
    void ClipRectangle(gdouble x0, gdouble x1, gdouble y0, gdouble y1);
    void PathCircle(gdouble x, gdouble y, gdouble r);
    void PathRect(gdouble x0, gdouble y0, gdouble x1, gdouble y1);
    void PathLine(gdouble x1, gdouble y1, gdouble x2, gdouble y2);
    void PathPolyline(gint n, gdouble *x, gdouble *y);
    void PathPolygon(gint n, gdouble *x, gdouble *y);
    void Stroke(pGEcontext gc);
    void StrokeAndFill(pGEcontext gc, gboolean allowZeroAlpha = TRUE);
    void SetFont(pGEcontext gc, pDevDesc dd);
    gdouble GetStrWidth(gchar *str_utf8, pGEcontext gc, pDevDesc dd);
    void ShowText(gdouble x, gdouble y, gchar *str_utf8, gdouble rot, gdouble hadj, pGEcontext gc, pDevDesc dd);

    // Merge layers
    cairo_surface_t* GetDispSurface() { return dispSurface; }
    cairo_surface_t* GetRecordSurface() { return recordSurface; }
    void MergeDispSurface(LayerStruct *layer);
    void MergeDispSurfaceWithTransformation(LayerStruct *layer, cairo_matrix_t *mt);

    // Replot
    void ReplotTo(cairo_t *cr, gboolean isVectorDevice);
    void PlotToDispSurface();
    void PlotToRecordSurface();
    void ReplotToDispSurface();
private:
    cairo_surface_t *dispSurface;
    cairo_surface_t *recordSurface;
    cairo_t *dispCr;
    cairo_t *recordCr;
    GString *layerName;
    GPtrArray *funRecorderBuffer;
    GPtrArray *parRecorderBuffer;
    const static gint RecorderBufferSize = 10000;
    GPtrArray *funRecorder;
    GPtrArray *parRecorder;
    const static gint RecorderSize = 100000;
    gint ID;

    // Layer size
    gdouble widthInPoints, heightInPoints;
    gint widthInPixels, heightInPixels;

    // Transformation parameters
    cairo_matrix_t transMat;
    gdouble scaleCenterX, scaleCenterY, scaleX, scaleY;
    gdouble rotCenterX, rotCenterY, rotAngle;
    gdouble transX, transY;

    // Utils for context functions
    void SetFillColorFromR(pGEcontext gc, gboolean allowZeroAlpha = TRUE);
    void SetLineColorFromR(pGEcontext gc);
    void SetLineTypeFromR(pGEcontext gc);

    // Pointer to basic context operation functions
    LSMemFunPtr fpCrSave;
    LSMemFunPtr fpCrRestore;
    LSMemFunPtr fpCrSetSourceRGBA;
    LSMemFunPtr fpCrSetOperatorSource;
    LSMemFunPtr fpCrSetLineWidth;
    LSMemFunPtr fpCrSetLineCap;
    LSMemFunPtr fpCrSetLineJoin;
    LSMemFunPtr fpCrSetMiterLimit;
    LSMemFunPtr fpCrSetDash;
    LSMemFunPtr fpCrRotate;
    LSMemFunPtr fpCrMoveTo;
    LSMemFunPtr fpCrRelMoveTo;
    LSMemFunPtr fpCrLineTo;
    LSMemFunPtr fpCrCircle;
    LSMemFunPtr fpCrRectangle;
    LSMemFunPtr fpCrClip;
    LSMemFunPtr fpCrResetClip;
    LSMemFunPtr fpCrClosePath;
    LSMemFunPtr fpCrStroke;
    LSMemFunPtr fpCrFillPreserve;
    LSMemFunPtr fpCrPaint;
    LSMemFunPtr fpCrSelectFontFace;
    LSMemFunPtr fpCrSetDefaultFontFace;
    LSMemFunPtr fpCrSetFontSize;
    LSMemFunPtr fpCrShowText;

    // Basic context operation functions
    inline void CrSave(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_save(dispCr);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSave));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);
    }
    inline void CrRestore(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_restore(dispCr);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrRestore));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);
    }
    inline void CrSetSourceRGBA(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_set_source_rgba(dispCr, tmp[0], tmp[1], tmp[2], tmp[3]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetSourceRGBA));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrSetOperatorSource(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_set_operator(dispCr, CAIRO_OPERATOR_SOURCE);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetOperatorSource));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrSetLineWidth(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_set_line_width(dispCr, tmp[0]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetLineWidth));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrSetLineCap(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gint *tmp = (gint *)pt;
            cairo_set_line_cap(dispCr, (cairo_line_cap_t)tmp[0]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetLineCap));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrSetLineJoin(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gint *tmp = (gint *)pt;
            cairo_set_line_join(dispCr, (cairo_line_join_t)tmp[0]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetLineJoin));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrSetMiterLimit(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_set_miter_limit(dispCr, tmp[0]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetMiterLimit));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrSetDash(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_set_dash(dispCr, tmp + 2, (gint)tmp[1], tmp[0]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetDash));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrRotate(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_rotate(dispCr, tmp[0]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrRotate));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrMoveTo(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_move_to(dispCr, tmp[0], tmp[1]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrMoveTo));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrRelMoveTo(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_rel_move_to(dispCr, tmp[0], 0.0);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrRelMoveTo));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrLineTo(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_line_to(dispCr, tmp[0], tmp[1]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrLineTo));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrCircle(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_arc(dispCr, tmp[0], tmp[1], tmp[2], 0, 2 * M_PI);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrCircle));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrRectangle(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_rectangle(dispCr, tmp[0], tmp[1], tmp[2], tmp[3]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrRectangle));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrClip(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_clip(dispCr);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrClip));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrResetClip(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_reset_clip(dispCr);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrResetClip));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrClosePath(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_close_path(dispCr);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrClosePath));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrStroke(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_stroke(dispCr);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrStroke));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrFillPreserve(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_fill_preserve(dispCr);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrFillPreserve));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrPaint(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_paint(dispCr);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrPaint));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrSelectFontFace(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gchar *tmp = (gchar *)pt;
            cairo_select_font_face(dispCr, tmp + 2, (cairo_font_slant_t)tmp[1],
                                   (cairo_font_weight_t)tmp[0]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSelectFontFace));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrSetDefaultFontFace(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_font_face_t *tmp = *(cairo_font_face_t **)pt;
            cairo_set_font_face(dispCr, tmp);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetDefaultFontFace));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);
    }
    inline void CrSetFontSize(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            gdouble *tmp = (gdouble *)pt;
            cairo_set_font_size(dispCr, tmp[0]);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrSetFontSize));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
    inline void CrShowText(void *pt, gboolean replot = FALSE)
    {
        if(replot)
        {
            cairo_show_text(dispCr, (const gchar *)pt);
            return;
        }
        g_ptr_array_add(funRecorderBuffer, (gpointer)(&fpCrShowText));
        g_ptr_array_add(parRecorderBuffer, (gpointer)pt);

    }
};

#endif // LAYERSTRUCT_H
