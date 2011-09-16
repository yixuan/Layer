#ifndef LAYER_H_INCLUDED
#define LAYER_H_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif



void R_gtk_setEventHandler();
SEXP set_default_font(SEXP fontPath);
SEXP do_layer_device(SEXP width, SEXP height, SEXP fontPath);
SEXP get_layers();
SEXP get_id_count();
SEXP add_layer(SEXP layerName);
SEXP get_current_layer();
SEXP set_current_layer_by_index(SEXP index);


/* Distinguish the definitions of TRUE and FALSE in R and GTK+ */

#ifdef TRUE
  #undef TRUE
  #define TRUE ((Rboolean)1)
#endif

#ifdef FALSE
  #undef FALSE
  #define FALSE ((Rboolean)0)
#endif



#ifdef __cplusplus
} /* End of extern "C" */
#endif


#endif // LAYER_H_INCLUDED
