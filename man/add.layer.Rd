\name{add.layer}
\alias{add.layer}
\title{Add a new layer}
\usage{
add.layer(layerName = NULL)
}
\arguments{
  \item{layerName}{the name of the newly added layer. If
  \code{NULL}, it will be assigned automatically.}
}
\value{
  A data frame listing the ID and name of the newly added
  layer.
}
\description{
  This function adds a new layer to the current active
  Layer device. If no Layer device is active, an error
  occurs.
}
\details{
  The newly created layer will be set as the "Current
  Layer". It means that consequent low-level plotting
  functions will be passed to this layer. (High-level
  plotting functions will destroy all layers and create a
  new one.)
}
\author{
  Yixuan Qiu (\email{yixuan.qiu@cos.name})
}
