#ifndef LAYERDEVICE_H
#define LAYERDEVICE_H


#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <R_ext/GraphicsEngine.h>
#include <R_ext/GraphicsDevice.h>

#include <gtk/gtk.h>

#include "Layer.h"


class LayerDesc;

class LayerDevice
{
public:
    LayerDevice();
    ~LayerDevice();


    void CreateDevice(gdouble wInInches, gdouble hInInches);
    void ConfigureDevice();
    void ShowDeviceWindow();
    void AddDeviceToR();

    // Device functions
    static void LayerActivate(pDevDesc dd);
    static void LayerDeactivate(pDevDesc dd);
    static void LayerNewPage(pGEcontext gc, pDevDesc dd);
    static void LayerClip(double x0, double x1, double y0, double y1, pDevDesc dd);
    static void LayerSize(double *left, double *right, double *bottom, double *top, pDevDesc dd);
    static void LayerCircle(double x, double y, double r, pGEcontext gc, pDevDesc dd);
    static void LayerLine(double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd);
    static void LayerPolygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd);
    static void LayerPolyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd);
    static void LayerRect(double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd);
    static void LayerText(double x, double y, const char *str, double rot, double hadj,
                          pGEcontext gc,
                          pDevDesc dd);
    static void LayerMetricInfo(int c, pGEcontext gc, double* ascent, double* descent,
                                double* width, pDevDesc dd);
    static double LayerStrWidth(const char *str, pGEcontext gc, pDevDesc dd);
    static void LayerMode(int mode, pDevDesc dd);
    static Rboolean LayerLocator(double *x, double *y, pDevDesc dd);
    static void LayerClose(pDevDesc dd);
private:
    // 1 inch = 25.4 mm
    static const gdouble mmPerInch = 25.4;
    // 1 inch = 72.0 points
    static const gdouble pointsPerInch = 72.0;

    pDevDesc dd;
    pGEDevDesc gdd;
    LayerDesc *lDesc;
};

#endif // LAYERDEVICE_H
