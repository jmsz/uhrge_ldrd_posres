for f in sixstrip_posresgrid
do
   echo "processing $f"
   cd /home/anp/Desktop/Simulation_package_v1_3D/grips_2/$f
#   make clean
    make
#   ./CreatePA ConfigFiles/SETUP_STRIP.txt
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Epot.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_00.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_01.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_02.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_03.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_04.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_05.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_06.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_07.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_08.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_09.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f  Strip_Wpot_10.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_00.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_01.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_02.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_03.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_04.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_05.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_06.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_07.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_08.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_09.pa
#   ../../adl3/tools/paPlotter/paPlotter.py -g 0.005 -f Strip_Stru_10.pa
   sleep 1
   ./SimulatePulse ConfigFiles/SETUP_STRIP.txt
   sleep 1
   ../../adl3/tools/pulsePlotter/pathPlotter.py -g 0.005 -f Strip_Wpot_03.pa
   ../../adl3/tools/pulsePlotter/timePlotter.py -g 0.005 -f TIMING_full_0.txt
   cd /home/anp/Desktop/Simulation_package_v1_3D/grips_2
done
