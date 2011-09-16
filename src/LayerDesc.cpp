#include "LayerStruct.h"
#include "LayerDesc.h"


LayerDesc::LayerDesc(gdouble wInPoints, gdouble hInPoints, gint wInPixels, gint hInPixels, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("LayerDesc Constructor\n");
#endif
    // Main window and drawing area
    gWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable(GTK_WINDOW(gWindow), FALSE);
    gDraw = gtk_drawing_area_new();
    gtk_widget_set_size_request(gDraw, wInPixels, hInPixels);
    GtkWidget *gVBox = gtk_vbox_new(FALSE, 0);
    gtk_box_pack_end(GTK_BOX(gVBox), gDraw, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(gWindow), gVBox);
    // Events for drawing area
    gtk_widget_add_events(gDraw, GDK_LEAVE_NOTIFY_MASK
                          | GDK_BUTTON_PRESS_MASK
                          | GDK_BUTTON_RELEASE_MASK
                          | GDK_POINTER_MOTION_MASK
                          | GDK_POINTER_MOTION_HINT_MASK);
    g_signal_connect(G_OBJECT(gDraw), "expose-event",
                     G_CALLBACK(OnExpose), dd);
    g_signal_connect(G_OBJECT(gDraw), "button_press_event",
                     G_CALLBACK(OnLeftMouseButtonPress), dd);
    g_signal_connect(G_OBJECT(gDraw), "motion_notify_event",
                     G_CALLBACK(OnMouseMove), dd);
    g_signal_connect(G_OBJECT(gDraw), "button_release_event",
                     G_CALLBACK(OnLeftMouseButtonRelease), dd);

    g_signal_connect(G_OBJECT(gWindow), "delete-event",
                     G_CALLBACK(OnDelete), dd);

    // Menu
    GtkItemFactoryEntry gEntry[] = {
        {(gchar *)"/File/Save As/PDF...",          NULL,
            (GtkItemFactoryCallback)LayerDesc::SaveAsCallback,     10,  (gchar *)"<Item>"},
        {(gchar *)"/File/Save As/PNG...",          NULL,
            (GtkItemFactoryCallback)LayerDesc::SaveAsCallback,     11,  (gchar *)"<Item>"},
        {(gchar *)"/File/Save As/SVG...",          NULL,
            (GtkItemFactoryCallback)LayerDesc::SaveAsCallback,     12,  (gchar *)"<Item>"},
        {(gchar *)"/File/Save As/PostScript...",   NULL,
            (GtkItemFactoryCallback)LayerDesc::SaveAsCallback,     13,  (gchar *)"<Item>"},
        {(gchar *)"/Transform/None",               NULL,
            (GtkItemFactoryCallback)LayerDesc::SetOpModeCallback,  -1,  (gchar *)"<RadioItem>"},
        {(gchar *)"/Transform/Move",               NULL,
            (GtkItemFactoryCallback)LayerDesc::SetOpModeCallback,   0,  (gchar *)"/Transform/None"},
        {(gchar *)"/Transform/Rotate",             NULL,
            (GtkItemFactoryCallback)LayerDesc::SetOpModeCallback,   1,  (gchar *)"/Transform/None"},
        {(gchar *)"/Transform/Scale",              NULL,
            (GtkItemFactoryCallback)LayerDesc::SetOpModeCallback,   2,  (gchar *)"/Transform/None"}
    };
    GtkItemFactory *gItemFac = gtk_item_factory_new(GTK_TYPE_MENU_BAR, "<main>", NULL);
    gtk_item_factory_create_items(gItemFac, 8, gEntry, (gpointer)this);
    GtkWidget *gMainMenu = gtk_item_factory_get_widget(gItemFac, "<main>");
    gtk_box_pack_start(GTK_BOX(gVBox), gMainMenu, FALSE, FALSE, 0);

    widthInPoints = wInPoints;
    heightInPoints = hInPoints;
    widthInPixels = wInPixels;
    heightInPixels = hInPixels;
    opMode = -1;
    mousePressX = mousePressY = 0;
    mouseHoldOn = FALSE;
    currentLayerIndex = 0;
    idCount = 1000;

    FT_Init_FreeType(&library);
    face = NULL;
    defaultFontFace = NULL;

    // Create layer structure
    layers.push_back(new LayerStruct("Layer_0", idCount, wInPoints, hInPoints, wInPixels, hInPixels));
    idCount++;
    currentLayer = layers[currentLayerIndex];
    background = new LayerStruct("background", -1, wInPoints, hInPoints, wInPixels, hInPixels);
    canvas = new LayerStruct("canvas", -1, wInPoints, hInPoints, wInPixels, hInPixels);
    canvas->SetScale(1.0, 1.0);
}

LayerDesc::~LayerDesc()
{
#ifdef LAYER_DEBUG
Rprintf("LayerDesc Destructor\n");
#endif
    if(defaultFontFace) cairo_font_face_destroy(defaultFontFace);
    if(face) FT_Done_Face(face);
    if(library) FT_Done_FreeType(library);

    if(gDraw) gtk_widget_destroy(gDraw);
    if(gWindow) gtk_widget_destroy(gWindow);
    currentLayer = NULL;
    for(size_t i = 0; i < layers.size(); i++)
    {
        delete layers[i];
    }
    delete background;
    delete canvas;
}





void LayerDesc::SetDefaultFontFace(const gchar *fontPath)
{
    if(face) FT_Done_Face(face);
    FT_New_Face(library, fontPath, 0, &face);
    if(defaultFontFace) cairo_font_face_destroy(defaultFontFace);
    defaultFontFace = cairo_ft_font_face_create_for_ft_face(face, 0);
}





gint LayerDesc::AddLayer(const gchar *layerName)
{
    layers.insert(layers.begin() + currentLayerIndex + 1,
                  new LayerStruct(layerName, idCount, widthInPoints, heightInPoints, widthInPixels, heightInPixels));
    idCount++;
    currentLayerIndex++;
    currentLayer = layers[currentLayerIndex];
    return idCount - 1;
}





void LayerDesc::SetDefaultTransMatrix()
{
    background->SetDefaultTransMatrix();
    for(size_t i = 0; i < layers.size(); i++)
    {
        (layers[i])->SetDefaultTransMatrix();
    }
}

void LayerDesc::PlotToDispSurface()
{
    background->PlotToDispSurface();
    for(size_t i = 0; i < layers.size(); i++)
    {
        (layers[i])->PlotToDispSurface();
    }
}

void LayerDesc::ReplotToDispSurface()
{
    background->ReplotToDispSurface();
    for(size_t i = 0; i < layers.size(); i++)
    {
        (layers[i])->ReplotToDispSurface();
    }
}

void LayerDesc::ReplotTo(cairo_t *cr, gboolean isVectorDevice)
{
    background->ReplotTo(cr, isVectorDevice);
    for(size_t i = 0; i < layers.size(); i++)
    {
        (layers[i])->ReplotTo(cr, isVectorDevice);
    }
}

void LayerDesc::MergeLayers()
{
    canvas->Clear();
    canvas->MergeDispSurface(background);
    for(size_t i = 0; i < layers.size(); i++)
    {
        canvas->MergeDispSurface(layers[i]);
    }
}

void LayerDesc::MergeLayersWithTransformation()
{
    canvas->Clear();
    cairo_matrix_t trans;
    trans = background->GetTransMatrix();
    canvas->MergeDispSurfaceWithTransformation(background, &trans);
    for(size_t i = 0; i < layers.size(); i++)
    {
        trans = (layers[i])->GetTransMatrix();
        canvas->MergeDispSurfaceWithTransformation(layers[i], &trans);
    }
}

void LayerDesc::CanvasToScreen()
{
    cairo_t *drawingCr = gdk_cairo_create(gDraw->window);
    cairo_set_operator(drawingCr, CAIRO_OPERATOR_SOURCE);
    cairo_set_source_surface(drawingCr, canvas->GetDispSurface(), 0.0, 0.0);
    cairo_paint(drawingCr);
    cairo_destroy(drawingCr);
}

void LayerDesc::ResetLayers()
{
    for(size_t i = 0; i < layers.size(); i++)
    {
        delete layers[i];
    }
    layers.clear();
    layers.push_back(new LayerStruct("Layer_0", idCount, widthInPoints, heightInPoints, widthInPixels, heightInPixels));
    idCount++;
    currentLayerIndex = 0;
    currentLayer = layers[currentLayerIndex];
}





gboolean LayerDesc::OnExpose(GtkWidget *widget, GdkEventExpose *event, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->MergeLayers();
    ld->CanvasToScreen();

    return TRUE;
}

gboolean LayerDesc::OnLeftMouseButtonPress(GtkWidget *widget, GdkEventButton *event, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->SetMousePressCoord(event->x, event->y);
    ld->SetMouseHoldOn(TRUE);

    return TRUE;
}

gboolean LayerDesc::OnMouseMove(GtkWidget *widget, GdkEventMotion *event, pDevDesc dd)
{
    gint x, y;
    GdkModifierType state;
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    if(!ld->IsMouseHoldOn()) return TRUE;

    gdk_window_get_pointer(event->window, &x, &y, &state);
    if(state & GDK_BUTTON1_MASK)
    {
        if(ld->opMode < 0) return TRUE;
        if(ld->opMode == 0)
        {
            ld->currentLayer->Move(x - ld->GetMousePressCoordX(),
                                   y - ld->GetMousePressCoordY());
        } else if (ld->opMode == 1) {
            gdouble v1x = ld->GetMousePressCoordX() - ld->currentLayer->GetRotateCenterX(),
                    v1y = ld->GetMousePressCoordY() - ld->currentLayer->GetRotateCenterY(),
                    v2x = x - ld->currentLayer->GetRotateCenterX(),
                    v2y = y - ld->currentLayer->GetRotateCenterY();
            gdouble r1 = sqrt(v1x * v1x + v1y * v1y), r2 = sqrt(v2x * v2x + v2y * v2y);
            gdouble cosa1 = v1x / r1, sina1 = v1y / r1, cosa2 = v2x / r2, sina2 = v2y / r2;
            gdouble cosa = cosa2 * cosa1 + sina2 * sina1, sina = sina2 * cosa1 - cosa2 * sina1;
            ld->currentLayer->Rotate(cosa, sina);
        } else if(ld->opMode == 2) {
            gdouble v1x = ld->GetMousePressCoordX() - ld->currentLayer->GetScaleCenterX(),
                    v1y = ld->GetMousePressCoordY() - ld->currentLayer->GetScaleCenterY(),
                    v2x = x - ld->currentLayer->GetScaleCenterX(),
                    v2y = y - ld->currentLayer->GetScaleCenterY();
            gdouble r1 = sqrt(v1x * v1x + v1y * v1y), r2 = sqrt(v2x * v2x + v2y * v2y);
            r1 = r1 < 10 ? 10 : r1;
            ld->currentLayer->Scale(r2 / r1, r2 / r1);
        }
        ld->SetMousePressCoord(x, y);
        ld->MergeLayersWithTransformation();
        ld->CanvasToScreen();
    }
    return TRUE;
}

gboolean LayerDesc::OnLeftMouseButtonRelease(GtkWidget *widget, GdkEventButton *event, pDevDesc dd)
{
    LayerDesc *ld = (LayerDesc *) dd->deviceSpecific;
    ld->SetMouseHoldOn(FALSE);
    ld->ReplotToDispSurface();
    ld->MergeLayers();
    ld->CanvasToScreen();

    return TRUE;
}

gboolean LayerDesc::OnDelete(GtkWidget *widget, GdkEvent *event, pDevDesc dd)
{
    GEkillDevice(desc2GEDesc(dd));

    return TRUE;
}





void LayerDesc::SetWindowActive(gboolean active, pDevDesc dd)
{
    GString *title = g_string_new("");
    if(active)
    {
        g_string_printf(title, "R Graphics: Layer Device %d (ACTIVE)", ndevNumber(dd) + 1);
    }
    else
    {
        g_string_printf(title, "R Graphics: Layer Device %d (INACTIVE)", ndevNumber(dd) + 1);
    }
    gtk_window_set_title(GTK_WINDOW(gWindow), title->str);
    g_string_free(title, TRUE);
}

void LayerDesc::NewPage(pGEcontext gc, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("NewPage\n");
#endif
    this->ResetLayers();
    background->PaintWithBgColor(gc, FALSE);
    this->MergeLayers();
    this->CanvasToScreen();
}

void LayerDesc::Clip(double x0, double x1, double y0, double y1, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Clip\n");
#endif
    currentLayer->ClipRectangle(x0, x1, y0, y1);
}

void LayerDesc::Size(double *left, double *right, double *bottom, double *top, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Size\n");
#endif
    *left = dd->left;
    *right = dd->right;
    *bottom = dd->bottom;
    *top = dd->top;
}

void LayerDesc::Circle(double x, double y, double r, pGEcontext gc, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Circle\n");
#endif
    currentLayer->PathCircle(x, y, r);
    currentLayer->StrokeAndFill(gc);
}

void LayerDesc::Line(double x1, double y1, double x2, double y2, pGEcontext gc, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Line\n");
#endif
    currentLayer->PathLine(x1, y1, x2, y2);
    currentLayer->Stroke(gc);
}

void LayerDesc::Polygon(int n, double *x, double *y, pGEcontext gc, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Polygon\n");
#endif
    currentLayer->PathPolygon(n, x, y);
    currentLayer->StrokeAndFill(gc);
}

void LayerDesc::Polyline(int n, double *x, double *y, pGEcontext gc, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Polyline\n");
#endif
    currentLayer->PathPolyline(n, x, y);
    currentLayer->Stroke(gc);
}

void LayerDesc::Rect(double x0, double y0, double x1, double y1, pGEcontext gc, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Rect\n");
#endif
    currentLayer->PathRect(x0, y0, x1, y1);
    currentLayer->StrokeAndFill(gc);
}

void LayerDesc::Text(double x, double y, const char *str, double rot, double hadj,
                     pGEcontext gc, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Text\n");
#endif
    gchar *str_utf8 = g_locale_to_utf8(str, -1, NULL, NULL, NULL);
    currentLayer->ShowText(x, y, str_utf8, rot, hadj, gc, dd);
}

void LayerDesc::MetricInfo(int c, pGEcontext gc, double* ascent, double* descent,
                           double* width, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("MetricInfo\n");
#endif
    *ascent = 0.0;
    *descent = 0.0;
    *width = 0.0;
}

double LayerDesc::StrWidth(const char *str, pGEcontext gc, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("StrWidth\n");
#endif
    currentLayer->SetFont(gc, dd);
    gchar *str_utf8 = g_locale_to_utf8(str, -1, NULL, NULL, NULL);
    return currentLayer->GetStrWidth(str_utf8, gc, dd);
}

void LayerDesc::Mode(int mode, pDevDesc dd)
{
#ifdef LAYER_DEBUG
Rprintf("Mode\n");
#endif
    if(mode == 0)
    {
        this->PlotToDispSurface();
        this->MergeLayers();
        this->CanvasToScreen();
    }
}

Rboolean LayerDesc::Locator(double *x, double *y, pDevDesc dd)
{
    return FALSE;
}





void LayerDesc::OpenSaveAsDialog(FileType fType, gchar *fName)
{
    gDialog = gtk_file_chooser_dialog_new("Save Graph",
				                          GTK_WINDOW(gWindow),
				                          GTK_FILE_CHOOSER_ACTION_SAVE,
                                          GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				                          GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                          NULL);
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(gDialog), TRUE);
    GtkFileFilter *filter = gtk_file_filter_new();
    gchar suffix[10];
    gchar suggest[128];
    switch(fType)
    {
        case fileTypePDF:
            gtk_file_filter_set_name(filter, "PDF files (*.pdf)");
            gtk_file_filter_add_pattern(filter, "*.pdf");
            g_snprintf(suffix, 10, "%s", ".pdf");
            break;
        case fileTypePNG:
            gtk_file_filter_set_name(filter, "PNG files (*.png)");
            gtk_file_filter_add_pattern(filter, "*.png");
            g_snprintf(suffix, 10, "%s", ".png");
            break;
        case fileTypeSVG:
            gtk_file_filter_set_name(filter, "SVG files (*.svg)");
            gtk_file_filter_add_pattern(filter, "*.svg");
            g_snprintf(suffix, 10, "%s", ".svg");
            break;
        case fileTypePS:
            gtk_file_filter_set_name(filter, "Postscript files (*.ps)");
            gtk_file_filter_add_pattern(filter, "*.ps");
            g_snprintf(suffix, 10, "%s", ".ps");
            break;
        default:
            break;
    }
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(gDialog), filter);
    g_snprintf(suggest, 10, "Rplot%s", suffix);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(gDialog), suggest);
    if(gtk_dialog_run(GTK_DIALOG(gDialog)) == GTK_RESPONSE_ACCEPT)
    {
        gchar *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(gDialog));
        if(g_str_has_suffix(filename, suffix))
            g_snprintf(fName, FILENAME_MAX, "%s", filename);
        else
            g_snprintf(fName, FILENAME_MAX, "%s%s", filename, suffix);
        g_free(filename);
    }
    gtk_widget_destroy(gDialog);
}

void LayerDesc::SaveAsFile(FileType fType, const gchar *fileName)
{
    cairo_surface_t *fileSurface = NULL;
    cairo_t *fileCr;
    switch(fType)
    {
        case fileTypePDF:
            fileSurface = cairo_pdf_surface_create(fileName, widthInPoints, heightInPoints);
            break;
        case fileTypePNG:
            fileSurface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
                                                     widthInPixels, heightInPixels);
            break;
        case fileTypeSVG:
            fileSurface = cairo_svg_surface_create(fileName, widthInPoints, heightInPoints);
            break;
        case fileTypePS:
            fileSurface = cairo_ps_surface_create(fileName, widthInPoints, heightInPoints);
            break;
        default:
            break;
    }
    fileCr = cairo_create(fileSurface);
    switch(fType)
    {
        case fileTypePDF:
            this->ReplotTo(fileCr, TRUE);
            cairo_show_page(fileCr);
            break;
        case fileTypePNG:
            this->ReplotTo(fileCr, FALSE);
            cairo_surface_write_to_png(fileSurface, fileName);
            break;
        case fileTypeSVG:
            this->ReplotTo(fileCr, TRUE);
            break;
        case fileTypePS:
            this->ReplotTo(fileCr, TRUE);
            break;
        default:
            break;
    }
    cairo_destroy(fileCr);
    cairo_surface_destroy(fileSurface);
}

void LayerDesc::SaveAsCallback(gpointer data, guint action, GtkWidget *widget)
{
    LayerDesc *ld = (LayerDesc *)data;
    FileType fType = (FileType) (action - 10);
    gchar fName[FILENAME_MAX];
    ld->OpenSaveAsDialog(fType, fName);
    ld->SaveAsFile(fType, fName);
}

void LayerDesc::SetOpModeCallback(gpointer data, guint action, GtkWidget *widget)
{
    LayerDesc *ld = (LayerDesc *)data;
    ld->SetOpMode(action);
}


