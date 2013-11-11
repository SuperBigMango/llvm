set term png
set output "mpeg.png"
set   autoscale                        # scale axes automatically
unset log                              # remove any log-scaling
unset label                            # remove any previous labels
set xtic auto                          # set xtics automatically
set ytic auto                          # set ytics automatically
set title "Memory Foot Print for mpeg2dec.bc"
set xlabel "Function #"
set ylabel "Memory used (in Bytes)"
plot    "mpeg_plot"  w l
