set term png
set output "jpeg.png"
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Memory Foot Print for jpeg-6a.bc"
set xlabel "Function #"
set ylabel "Memory used (in Bytes)"
plot    "jpeg_plot"  w l
