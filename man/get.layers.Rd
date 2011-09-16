\name{get.layers}
\alias{get.layers}
\title{Get layer information}
\value{
  A data frame listing the ID and name of all available
  layers.
}
\description{
  This function returns the IDs and names of all available
  layers on the active Layer device. If no Layer device is
  active, an error occurs.
}
\details{
  In Layer device every layer has been assigned an ID
  number and a name. ID is an integer to uniquely identify
  a layer, and it is created automatically by the device.
  The name is just like a note of the content of the layer,
  so it can be any character string.
}
\author{
  Yixuan Qiu (\email{yixuan.qiu@cos.name})
}
