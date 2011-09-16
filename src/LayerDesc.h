#ifndef LAYERDESC_H
#define LAYERDESC_H


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

#include <vector>
#include "Layer.h"
#include "LayerStruct.h"


enum FileType{fileTypePDF, fileTypePNG, fileTypeSVG, fileTypePS};

class LayerDesc
{
public:
    LayerDesc(gdouble wInPoints, gdouble hInPoints, gint wInPixels, gint hInPixels, pDevDesc dd);
    ~LayerDesc();

    inline void ShowWindow() { gtk_widget_show_all(gWindow); }
    inline gdouble GetWidthInPoints() { return widthInPoints; }
    inline gdouble GetHeightInPoints() { return heightInPoints; }

    void SetDefaultFontFace(const gchar *fontPath);
    inline cairo_font_face_t* GetDefaultFontFace() { return defaultFontFace; }

    gint AddLayer(const gchar* layerName);
    inline gint GetIdCount() { return idCount; }
    inline gint GetLayersCount() { return (gint) layers.size(); }
    inline const gchar* GetLayerNameAt(gint index) { return layers[index]->GetLayerName(); }
    inline gint GetLayerIdAt(gint index) { return layers[index]->GetLayerId(); }
    inline gint GetCurrentLayerIndex() { return currentLayerIndex; }
    inline void SetCurrentLayerByIndex(gint index) { currentLayerIndex = index; currentLayer = layers[currentLayerIndex]; }

    inline void SetOpMode(gint operateMode) { opMode = operateMode; }
    void SetDefaultTransMatrix();
    void PlotToDispSurface();
    void ReplotToDispSurface();
    void ReplotTo(cairo_t *cr, gboolean isVectorDevice);
    void MergeLayers();
    void MergeLayersWithTransformation();
    void CanvasToScreen();
    void ResetLayers();

    inline void SetMousePressCoord(gint x, gint y) { mousePressX = x; mousePressY = y; }
    inline void SetMouseHoldOn(gboolean isHoldOn) { mouseHoldOn = isHoldOn; }
    inline gboolean IsMouseHoldOn() { return mouseHoldOn; }
    inline gint GetMousePressCoordX() {return mousePressX;}
    inline gint GetMousePressCoordY() {return mousePressY;}

    void OpenSaveAsDialog(FileType fType, gchar *fName);

    void Activate(pDevDesc dd) { SetWindowActive(TRUE, dd); }
    void Deactivate(pDevDesc dd) { SetWindowActive(FALSE, dd); }
    void NewPage(pGEcontext gc, pDevDesc dd);
    void Clip(double x0, double x1, double y0, double y1, pDevDesc dd);
    void Size(double *left, double *right, double *bottom, double *top, pDevDesc dd);
    void Circle(double x, double y, double r, pGEcontext gc, pDevDesc dd);
    void Line(double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd);
    void Polygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd);
    void Polyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd);
    void Rect(double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd);
    void Text(double x, double y, const char *str, double rot, double hadj,
              pGEcontext gc,
              pDevDesc dd);
    void MetricInfo(int c, pGEcontext gc, double* ascent, double* descent,
                    double* width, pDevDesc dd);
    double StrWidth(const char *str, pGEcontext gc, pDevDesc dd);
    void Mode(int mode, pDevDesc dd);
    Rboolean Locator(double *x, double *y, pDevDesc dd);
private:
    // Main Window
    GtkWidget *gWindow;
    // Drawing widget
    GtkWidget *gDraw;
    // Save file dialog
    GtkWidget *gDialog;
    // Layer structures
    std::vector<LayerStruct*> layers;
    // Background layer
    LayerStruct *background;
    // Record merged layers and render the drawing widget
    LayerStruct *canvas;
    // Pointer to current layer
    LayerStruct *currentLayer;
    // Size of the device
    gdouble widthInPoints, heightInPoints;
    gint widthInPixels, heightInPixels;
    // Operation mode: -1 for none, 0 for move, 1 for rotate, 2 for scale
    gint opMode;
    // Coordinates in button events
    gint mousePressX, mousePressY;
    gboolean mouseHoldOn;
    // Number of available layers; ID of current layer
    gint currentLayerIndex, idCount;
    // Default font
    FT_Library library;
    FT_Face face;
    cairo_font_face_t *defaultFontFace;

    void SetWindowActive(gboolean active, pDevDesc dd);

    // Events
    static gboolean OnExpose(GtkWidget *widget, GdkEventExpose *event, pDevDesc dd);
    static gboolean OnLeftMouseButtonPress(GtkWidget *widget, GdkEventButton *event, pDevDesc dd);
    static gboolean OnMouseMove(GtkWidget *widget, GdkEventMotion *event, pDevDesc dd);
    static gboolean OnLeftMouseButtonRelease(GtkWidget *widget, GdkEventButton *event, pDevDesc dd);
    static gboolean OnDelete(GtkWidget *widget, GdkEvent *event, pDevDesc dd);

    // Menu item callbacks
    void SaveAsFile(FileType fType, const gchar *fileName);
    static void SaveAsCallback(gpointer data, guint action, GtkWidget *widget);
    static void SaveAsPDFCallback(gpointer data, guint action, GtkWidget *widget);
    static void SaveAsPNGCallback(gpointer data, guint action, GtkWidget *widget);
    static void SaveAsSVGCallback(gpointer data, guint action, GtkWidget *widget);
    static void SaveAsPSCallback(gpointer data, guint action, GtkWidget *widget);
    static void SetOpModeCallback(gpointer data, guint action, GtkWidget *widget);
};

#endif // LAYERDESC_H
