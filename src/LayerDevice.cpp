#include "LayerDesc.h"
#include "LayerDevice.h"


LayerDevice::LayerDevice()
{
#ifdef LAYER_DEBUG
Rprintf("LayerDevice Constructor\n");
#endif
    gint argc = 1;
    gchar *s = g_new0(gchar, 2);
    s[0] = 'R';
    gchar **argv = &s;
    gtk_init(&argc, &argv);
    R_gtk_setEventHandler();

    dd = g_new0(DevDesc, 1);
}

LayerDevice::~LayerDevice()
{
#ifdef LAYER_DEBUG
Rprintf("LayerDevice Destructor\n");
#endif
}


void LayerDevice::CreateDevice(gdouble wInInches, gdouble hInInches)
{
    // Compute resolution
    gdouble resolutionX = mmPerInch * gdk_screen_width() / gdk_screen_width_mm();
    gdouble resolutionY = mmPerInch * gdk_screen_height() / gdk_screen_height_mm();
    gdouble wInPoints = wInInches * pointsPerInch;
    gdouble hInPoints = hInInches * pointsPerInch;
    gint wInPixels = (gint)(wInInches * resolutionX);
    gint hInPixels = (gint)(hInInches * resolutionY);
    lDesc = new LayerDesc(wInPoints, hInPoints, wInPixels, hInPixels, dd);
}

void LayerDevice::ConfigureDevice()
{
    dd->deviceSpecific = (void *) lDesc;
    // Device functions
    dd->close = LayerClose;
    dd->activate = LayerActivate;
    dd->deactivate = LayerDeactivate;
    dd->size = LayerSize;
    dd->newPage = LayerNewPage;
    dd->clip = LayerClip;
    dd->strWidth = LayerStrWidth;
    dd->text = LayerText;
    dd->rect = LayerRect;
    dd->circle = LayerCircle;
    dd->line = LayerLine;
    dd->polyline = LayerPolyline;
    dd->polygon = LayerPolygon;
    dd->locator = LayerLocator;
    dd->mode = LayerMode;
    dd->metricInfo = LayerMetricInfo;
    // Initial graphical settings
    dd->startfont = 1;
    dd->startps = 10;
    dd->startcol = R_RGB(0, 0, 0);
    dd->startfill = R_TRANWHITE;
    dd->startlty = LTY_SOLID;
    dd->startgamma = 1;
    // Device physical characteristics
    dd->left = 0;
    dd->right = lDesc->GetWidthInPoints();
    dd->bottom = lDesc->GetHeightInPoints();
    dd->top = 0;
    dd->cra[0] = 10;
    dd->cra[1] = 12;
    dd->xCharOffset = 0.4900;
    dd->yCharOffset = 0.3333;
    dd->yLineBias = 0.1;
    dd->ipr[0] = 1.0 / pointsPerInch;
    dd->ipr[1] = 1.0 / pointsPerInch;
    // Device capabilities
    dd->canClip = TRUE;
    dd->canHAdj = 2;
    dd->canChangeGamma = FALSE;
    dd->displayListOn = TRUE;
}

void LayerDevice::ShowDeviceWindow()
{
    lDesc->ShowWindow();
}

void LayerDevice::AddDeviceToR()
{
    R_GE_checkVersionOrDie(R_GE_version);
    R_CheckDeviceAvailable();
    BEGIN_SUSPEND_INTERRUPTS
    {
        gdd = GEcreateDevDesc(dd);
        GEaddDevice2(gdd, "Layer");
    } END_SUSPEND_INTERRUPTS;
    gdk_flush();
}

void LayerDevice::LayerActivate(pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Activate(dd);
}

void LayerDevice::LayerDeactivate(pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Deactivate(dd);
}

void LayerDevice::LayerNewPage(pGEcontext gc, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->NewPage(gc, dd);
}

void LayerDevice::LayerClip(double x0, double x1, double y0, double y1, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Clip(x0, x1, y0, y1, dd);
}

void LayerDevice::LayerSize(double *left, double *right, double *bottom, double *top, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Size(left, right, bottom, top, dd);
}

void LayerDevice::LayerCircle(double x, double y, double r, pGEcontext gc, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Circle(x, y, r, gc, dd);
}

void LayerDevice::LayerLine(double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Line(x1, y1, x2, y2, gc, dd);
}

void LayerDevice::LayerPolygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Polygon(n, x, y, gc, dd);
}

void LayerDevice::LayerPolyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Polyline(n, x, y, gc, dd);
}

void LayerDevice::LayerRect(double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Rect(x0, y0, x1, y1, gc, dd);
}

void LayerDevice::LayerText(double x, double y, const char *str, double rot, double hadj,
                            pGEcontext gc,
                            pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Text(x, y, str, rot, hadj, gc, dd);
}

void LayerDevice::LayerMetricInfo(int c, pGEcontext gc, double* ascent, double* descent,
                                  double* width, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->MetricInfo(c, gc, ascent, descent, width, dd);
}

double LayerDevice::LayerStrWidth(const char *str, pGEcontext gc, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    return ld->StrWidth(str, gc, dd);
}

void LayerDevice::LayerMode(int mode, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->Mode(mode, dd);
}

Rboolean LayerDevice::LayerLocator(double *x, double *y, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    return ld->Locator(x, y, dd);
}

void LayerDevice::LayerClose(pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Close\n");
#endif
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    dd->deviceSpecific = NULL;
    delete ld;
}

