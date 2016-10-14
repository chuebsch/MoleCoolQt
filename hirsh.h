#include "molekul.h"
#include "moliso.h"

typedef struct SFAC{
 char lab[12];
 int prot,elec;
 double mass;
 double ab6[12];
} SFAC;

typedef struct HiAtom{
  int iscat;
  V3 frac,oros[6];
  Matrix M,Mi[6],Bij;
  double occ,pref[6];
}HiAtom;

struct BMhead{
  int h,sd,d,nn1,nn2,nn3;
  double cel[7];
  int f;
    int sid;
  };

const SFAC formfactors[115]={
 {"H", 1 ,1 ,1.007900 ,{0.430280 ,23.023120 ,0.285370 ,10.201380 ,0.171340 ,51.254440 ,0.094510 ,4.135110 ,0.017250 ,1.354270 ,0.001140 ,0.242690}},
 {"He", 2 ,2 ,4.002600 ,{0.694750 ,5.833660 ,0.620680 ,12.876820 ,0.386610 ,2.532960 ,0.152230 ,28.161710 ,0.126610 ,0.975070 ,0.019070 ,0.253080}},
 {"Li", 3 ,3 ,6.941000 ,{0.846450 ,4.632530 ,0.811460 ,1.718620 ,0.810960 ,97.873640 ,0.261150 ,0.506200 ,0.260550 ,200.000000 ,0.009300 ,0.000100}},
 {"Li+", 3 ,2 ,6.941000 ,{0.793750 ,2.886780 ,0.547360 ,1.169050 ,0.461610 ,6.182500 ,0.139180 ,0.317150 ,0.058000 ,12.609830 ,0.000100 ,28.159270}},
 {"Be", 4 ,4 ,9.012200 ,{1.592610 ,43.673970 ,1.127680 ,1.862750 ,0.702960 ,0.542430 ,0.538150 ,103.449100 ,0.038630 ,0.000100 ,0.000100 ,0.349750}},
 {"Be2+", 4 ,2 ,9.012200 ,{0.825770 ,2.042120 ,0.736910 ,0.802520 ,0.235570 ,4.601570 ,0.201350 ,0.211620 ,0.000340 ,43.682580 ,0.000100 ,103.455100}},
 {"B", 5 ,5 ,10.810000 ,{2.074180 ,23.395430 ,1.205770 ,1.076720 ,1.075920 ,60.932490 ,0.520230 ,0.271320 ,0.122800 ,0.271920 ,0.000100 ,0.113610}},
 {"C", 6 ,6 ,12.011000 ,{2.099210 ,13.189970 ,1.808320 ,30.379560 ,1.261590 ,0.692550 ,0.567750 ,0.163810 ,0.263030 ,68.427740 ,0.000100 ,0.440830}},
 {"Cv", 6 ,6 ,12.011000 ,{2.034920 ,25.996750 ,1.642860 ,11.778090 ,0.680600 ,0.510130 ,0.670220 ,0.978660 ,0.516500 ,0.169150 ,0.454880 ,57.918740}},
 {"N", 7 ,7 ,14.006700 ,{2.454240 ,18.666940 ,2.157820 ,8.312710 ,1.057820 ,0.469890 ,0.575570 ,42.446460 ,0.449590 ,0.087470 ,0.304800 ,0.471260}},
 {"O", 8 ,8 ,15.999400 ,{2.347520 ,9.697100 ,1.830060 ,18.598760 ,1.615380 ,5.198790 ,1.524020 ,0.324080 ,0.414230 ,39.790990 ,0.268670 ,0.011500}},
 {"O-", 8 ,9 ,15.999400 ,{3.563780 ,14.105610 ,2.149500 ,5.604910 ,1.527600 ,0.328010 ,1.479800 ,46.888620 ,0.270650 ,0.009800 ,0.000100 ,10.980840}},
 {"F", 9 ,9 ,18.998400 ,{2.969810 ,7.523650 ,2.045360 ,15.414410 ,1.781230 ,3.797210 ,1.520860 ,0.252090 ,0.422530 ,33.764780 ,0.260080 ,0.004880}},
 {"F-", 9 ,10 ,18.998400 ,{3.226840 ,4.959970 ,2.471110 ,14.459520 ,1.598390 ,0.172670 ,1.284900 ,11.396530 ,1.113350 ,43.308170 ,0.301820 ,0.967030}},
 {"Ne", 10 ,10 ,20.179700 ,{3.564130 ,7.305590 ,2.725590 ,3.344910 ,1.673590 ,15.932260 ,1.588840 ,0.138590 ,0.254680 ,0.691110 ,0.193200 ,35.263680}},
 {"Na", 11 ,11 ,22.989800 ,{4.164910 ,4.230960 ,2.380970 ,9.485020 ,1.704840 ,0.125590 ,1.596220 ,1.983580 ,0.662910 ,172.133300 ,0.489710 ,82.230910}},
 {"Na+", 11 ,10 ,22.989800 ,{3.695290 ,3.241830 ,3.304590 ,7.071790 ,1.683330 ,0.122790 ,0.691490 ,15.453340 ,0.624310 ,1.436640 ,0.000880 ,35.263830}},
 {"Mg", 12 ,12 ,24.305000 ,{3.908820 ,3.060410 ,2.621590 ,6.121460 ,1.691570 ,0.103570 ,1.526100 ,58.650220 ,1.479070 ,1.569400 ,0.772620 ,125.499800}},
 {"Mg2+", 12 ,10 ,24.305000 ,{4.303850 ,4.020450 ,2.583900 ,1.853040 ,1.713970 ,0.106930 ,1.393680 ,8.785230 ,0.004700 ,58.587120 ,0.000100 ,125.500500}},
 {"Al", 13 ,13 ,26.981500 ,{4.254740 ,3.766700 ,3.583010 ,1.691510 ,2.373510 ,45.278100 ,1.723660 ,0.092380 ,0.994000 ,113.969800 ,0.070310 ,17.479220}},
 {"Al3+", 13 ,10 ,26.981500 ,{4.193670 ,3.371340 ,3.000320 ,1.586370 ,1.715900 ,0.091580 ,1.088400 ,6.996790 ,0.001670 ,45.264560 ,0.000100 ,113.972700}},
 {"Si", 14 ,14 ,28.085500 ,{4.949760 ,2.702540 ,3.254030 ,34.453140 ,2.849570 ,1.240590 ,1.660530 ,0.072010 ,1.229490 ,84.536480 ,0.056110 ,56.342080}},
 {"Si0", 14 ,14 ,28.085500 ,{5.494880 ,2.608020 ,3.337700 ,37.462890 ,2.387650 ,1.096470 ,1.598640 ,0.064390 ,1.179860 ,80.523370 ,0.000100 ,56.270560}},
 {"Si4+", 14 ,10 ,28.085500 ,{3.983920 ,2.946480 ,3.536750 ,1.394880 ,1.728080 ,0.080690 ,0.751030 ,5.916040 ,0.000130 ,56.231760 ,0.000100 ,79.765800}},
 {"P", 15 ,15 ,30.973800 ,{6.481970 ,1.895370 ,4.316660 ,27.614550 ,1.737590 ,0.509910 ,1.357930 ,66.282960 ,1.105590 ,0.000100 ,0.000100 ,12.056520}},
 {"S", 16 ,16 ,32.066000 ,{6.905650 ,1.467640 ,5.244100 ,22.315760 ,1.545160 ,56.063280 ,1.429220 ,0.255880 ,0.875640 ,0.000100 ,0.000100 ,26.968920}},
 {"Cl", 17 ,17 ,35.452700 ,{7.133810 ,1.174550 ,6.269720 ,18.576260 ,1.826580 ,0.078690 ,1.625790 ,48.082030 ,0.144310 ,0.078710 ,0.000100 ,23.238940}},
 {"Cl-", 17 ,18 ,35.452700 ,{7.139320 ,1.180730 ,6.342130 ,19.529010 ,2.298010 ,61.048500 ,1.978260 ,0.080570 ,0.228540 ,23.182250 ,0.009830 ,0.097590}},
 {"Ar", 18 ,18 ,39.948000 ,{7.285510 ,15.632950 ,7.245490 ,0.955620 ,1.747750 ,0.044560 ,1.721740 ,41.075500 ,0.000100 ,0.006170 ,0.000100 ,20.096280}},
 {"K", 19 ,19 ,39.098300 ,{8.131610 ,12.736750 ,7.439720 ,0.774430 ,1.421590 ,0.000100 ,1.120300 ,200.000000 ,0.883420 ,36.187110 ,0.000100 ,82.983800}},
 {"K+", 19 ,18 ,39.098300 ,{8.003720 ,12.704760 ,7.440770 ,0.774730 ,1.422170 ,0.000100 ,1.134910 ,32.442700 ,0.000100 ,199.999000 ,0.000100 ,82.982980}},
 {"Ca", 20 ,20 ,40.078000 ,{8.629650 ,10.452380 ,7.387650 ,0.660360 ,1.630440 ,87.062580 ,1.376810 ,0.000100 ,0.975380 ,181.277600 ,0.000100 ,28.578900}},
 {"Ca2+", 20 ,18 ,40.078000 ,{8.668030 ,10.629550 ,7.397470 ,0.663060 ,1.383250 ,0.000100 ,0.553480 ,30.984760 ,0.000100 ,199.998800 ,0.000100 ,82.978980}},
 {"Sc", 21 ,21 ,44.955900 ,{9.188940 ,9.029480 ,7.367270 ,0.573640 ,1.602140 ,137.405000 ,1.336550 ,0.000100 ,0.783860 ,51.536150 ,0.720470 ,53.743950}},
 {"Sc3+", 21 ,18 ,44.955900 ,{9.013950 ,8.866580 ,7.364770 ,0.567710 ,1.321600 ,0.000100 ,0.301790 ,29.981330 ,0.000100 ,137.400300 ,0.000100 ,53.698110}},
 {"Ti", 22 ,22 ,47.880000 ,{9.758610 ,7.861720 ,7.353540 ,0.501070 ,1.468420 ,32.751460 ,1.405910 ,90.951310 ,1.286690 ,0.000100 ,0.726090 ,149.028700}},
 {"Ti2+", 22 ,20 ,47.880000 ,{9.676070 ,7.928580 ,7.358740 ,0.503880 ,1.667750 ,23.882140 ,1.296810 ,0.000100 ,0.000100 ,92.103880 ,0.000100 ,145.588100}},
 {"Ti3+", 22 ,19 ,47.880000 ,{9.563760 ,7.727290 ,7.353200 ,0.496040 ,1.269970 ,0.000100 ,0.814960 ,22.379310 ,0.000100 ,92.105600 ,0.000100 ,145.589200}},
 {"Ti4+", 22 ,18 ,47.880000 ,{9.223950 ,7.446340 ,7.351170 ,0.485950 ,1.233670 ,0.000100 ,0.193050 ,28.205120 ,0.000100 ,92.109300 ,0.000100 ,145.590100}},
 {"V", 23 ,23 ,50.941500 ,{10.254430 ,6.861770 ,7.346990 ,0.439390 ,1.840390 ,23.702590 ,1.721480 ,79.720530 ,1.226110 ,0.000100 ,0.610000 ,149.364900}},
 {"V2+", 23 ,21 ,50.941500 ,{10.142090 ,6.906150 ,7.350150 ,0.442240 ,2.253610 ,20.145750 ,1.238870 ,0.000100 ,0.015330 ,120.217000 ,0.000100 ,55.098120}},
 {"V3+", 23 ,20 ,50.941500 ,{10.057230 ,6.752900 ,7.348750 ,0.435090 ,1.387590 ,18.251220 ,1.207520 ,0.000100 ,0.000100 ,120.221500 ,0.000100 ,55.110620}},
 {"V5+", 23 ,18 ,50.941500 ,{9.376950 ,6.316250 ,7.363890 ,0.415680 ,1.116210 ,0.000100 ,0.144500 ,25.360440 ,0.000100 ,199.998700 ,0.000100 ,82.978470}},
 {"Cr", 24 ,24 ,51.996100 ,{10.672250 ,6.121430 ,4.620930 ,0.392930 ,3.331590 ,20.154700 ,2.727840 ,0.392930 ,1.452810 ,92.013170 ,1.190900 ,0.000100}},
 {"Cr2+", 24 ,22 ,51.996100 ,{10.541300 ,6.040090 ,4.414570 ,0.389670 ,2.934360 ,0.389660 ,2.870240 ,16.949380 ,1.172290 ,0.000100 ,0.067430 ,59.984000}},
 {"Cr3+", 24 ,21 ,51.996100 ,{10.455970 ,5.906410 ,4.436830 ,0.388630 ,2.925050 ,0.370410 ,2.061490 ,15.342210 ,1.119810 ,0.000100 ,0.001200 ,59.682710}},
 {"Mn", 25 ,25 ,54.938100 ,{10.985760 ,5.279510 ,7.356170 ,0.341990 ,2.920910 ,14.557910 ,1.657070 ,54.879000 ,1.080180 ,0.000100 ,0.999060 ,118.265100}},
 {"Mn2+", 25 ,23 ,54.938100 ,{10.865800 ,5.304500 ,7.354010 ,0.344870 ,3.492670 ,14.157180 ,1.099870 ,0.000100 ,0.185370 ,38.607300 ,0.002490 ,100.135600}},
 {"Mn3+", 25 ,22 ,54.938100 ,{11.044140 ,5.324620 ,4.436110 ,0.159710 ,4.067370 ,0.474880 ,2.445020 ,13.901080 ,0.005590 ,100.140200 ,0.001890 ,38.597230}},
 {"Mn4+", 25 ,21 ,54.938100 ,{10.807390 ,5.120310 ,7.378190 ,0.331810 ,1.805480 ,12.465890 ,1.009480 ,0.000100 ,0.000100 ,100.146600 ,0.000100 ,38.601850}},
 {"Fe", 26 ,26 ,55.847000 ,{11.188580 ,4.645990 ,7.372060 ,0.303270 ,3.551410 ,12.076550 ,1.681250 ,44.153160 ,1.208930 ,104.118700 ,0.996520 ,0.000100}},
 {"Fe2+", 26 ,24 ,55.847000 ,{11.323940 ,4.716110 ,7.358280 ,0.307930 ,4.085420 ,12.879000 ,1.039340 ,0.000240 ,0.194380 ,43.731180 ,0.000100 ,103.919200}},
 {"Fe3+", 26 ,23 ,55.847000 ,{11.276410 ,4.638940 ,7.375950 ,0.301690 ,3.320580 ,11.639080 ,0.984610 ,0.000100 ,0.042630 ,44.102890 ,0.000100 ,103.920700}},
 {"Co", 27 ,27 ,58.933200 ,{11.416240 ,4.122580 ,7.389020 ,0.270690 ,4.213510 ,10.366360 ,1.801890 ,38.324420 ,1.261030 ,97.149700 ,0.917100 ,0.000100}},
 {"Co2+", 27 ,25 ,58.933200 ,{11.595390 ,4.184740 ,7.376010 ,0.275100 ,4.751310 ,11.192060 ,0.958180 ,0.000100 ,0.318430 ,36.275090 ,0.000100 ,93.959330}},
 {"Co3+", 27 ,24 ,58.933200 ,{11.581350 ,4.131550 ,7.389640 ,0.270120 ,4.012010 ,10.326930 ,0.914190 ,0.000100 ,0.103530 ,35.203690 ,0.000100 ,93.959080}},
 {"Ni", 28 ,28 ,  58.690000 ,{11.763000 ,3.697290 ,7.398880 ,0.243740 ,4.854910 ,9.305930 ,1.980790 ,36.588800 ,1.148570 ,96.028750 ,0.853250 ,0.000100}},
 {"Ni2+", 28 ,26 ,58.690000 ,{11.838380 ,3.760400 ,5.164460 ,9.577070 ,4.592150 ,0.315570 ,3.728260 ,0.116460 ,0.677190 ,25.172860 ,0.000100 ,96.767030}},
 {"Ni3+", 28 ,25 ,58.690000 ,{12.089320 ,3.734860 ,7.370510 ,0.245880 ,4.533280 ,9.525240 ,0.893890 ,0.001000 ,0.114400 ,36.549980 ,0.000100 ,96.771100}},
 {"Cu", 29 ,29 ,  63.546000 ,{11.872110 ,3.347730 ,7.374910 ,0.225220 ,6.085480 ,8.461650 ,1.943370 ,27.950100 ,0.864750 ,98.021650 ,0.858370 ,0.000120}},
 {"Cu+", 29 ,28 , 63.546000 ,{11.749940 ,3.347140 ,6.772490 ,0.238310 ,6.212290 ,8.328200 ,1.755520 ,23.583460 ,1.475600 ,0.043310 ,0.034610 ,98.017380}},
 {"Cu2+", 29 ,27 ,63.546000 ,{11.831870 ,3.339650 ,5.781920 ,0.255300 ,5.775310 ,8.030310 ,2.460410 ,0.082010 ,1.146980 ,19.993270 ,0.003530 ,98.020900}},
 {"Zn", 30 ,30 ,  65.390000 ,{12.530200 ,3.058280 ,6.570920 ,0.143260 ,5.848800 ,7.589300 ,2.076100 ,28.507060 ,1.658930 ,0.383690 ,1.313880 ,82.220920}},
 {"Zn2+", 30 ,28 ,65.390000 ,{12.496090 ,3.525090 ,7.881480 ,0.166190 ,4.991900 ,9.205410 ,2.056020 ,1.713720 ,0.575050 ,24.204270 ,0.000100 ,82.219230}},
 {"Ga", 31 ,31 ,  69.723000 ,{10.698650 ,3.447870 ,7.891270 ,0.154260 ,4.747780 ,2.073870 ,3.831200 ,8.384410 ,2.592180 ,34.933560 ,1.237120 ,99.347320}},
 {"Ga3+", 31 ,28 ,69.723000 ,{10.801930 ,3.678000 ,7.894700 ,0.154680 ,5.306200 ,2.085100 ,3.911360 ,9.115680 ,0.086930 ,34.761550 ,0.000100 ,99.349530}},
 {"Ge", 32 ,32 ,  72.610000 ,{9.563350 ,2.214940 ,7.869940 ,0.142840 ,7.642150 ,3.864900 ,3.312960 ,32.694170 ,2.133510 ,8.942860 ,1.477040 ,82.158270}},
 {"Ge4+", 32 ,28 ,72.610000 ,{8.642380 ,3.758520 ,8.440150 ,2.145950 ,7.882100 ,0.143660 ,2.999850 ,8.162070 ,0.035900 ,30.935760 ,0.000100 ,72.314490}},
 {"As", 33 ,33 ,  74.921600 ,{10.862050 ,2.100460 ,7.832480 ,0.132090 ,5.488620 ,3.336310 ,4.212500 ,26.382540 ,2.569040 ,5.819920 ,2.034130 ,63.745670}},
 {"Se", 34 ,34 ,  78.960000 ,{12.638430 ,1.970060 ,7.773160 ,0.121670 ,5.806450 ,3.576090 ,4.442960 ,28.843480 ,1.828980 ,15.157660 ,1.509380 ,64.030250}},
 {"Br", 35 ,35 ,  79.904000 ,{12.568350 ,1.798260 ,7.706690 ,0.112040 ,5.762430 ,2.988480 ,4.780930 ,25.628560 ,2.484120 ,14.954200 ,1.696740 ,55.443290}},
 {"Br-", 35 ,36 , 79.904000 ,{14.728090 ,1.877810 ,7.733400 ,0.112850 ,4.081530 ,23.456500 ,3.899200 ,3.652070 ,2.849950 ,21.506460 ,2.704120 ,68.504300}},
 {"Kr", 36 ,36 ,  83.800000 ,{13.323730 ,1.673990 ,7.646450 ,0.103460 ,5.713510 ,17.436460 ,4.950090 ,2.625660 ,2.804270 ,42.879080 ,1.560380 ,19.802810}},
 {"Rb", 37 ,37 ,  85.470000 ,{17.739320 ,1.682980 ,7.704150 ,0.099440 ,5.334840 ,12.807390 ,4.928290 ,23.593430 ,1.286710 ,200.000000 ,0.000100 ,77.168060}},
 {"Rb+", 37 ,36 , 85.470000 ,{17.727360 ,1.682580 ,7.708460 ,0.099620 ,6.227070 ,13.347130 ,4.233200 ,25.648590 ,0.104560 ,76.909280 ,0.000100 ,199.998600}},
 {"Sr", 38 ,38 ,  87.620000 ,{11.779200 ,1.522660 ,9.534890 ,13.502710 ,7.571200 ,0.089950 ,6.030470 ,1.522510 ,2.026530 ,162.869700 ,1.056520 ,53.070680}},
 {"Sr2+", 38 ,36 ,87.620000 ,{13.562530 ,1.526390 ,9.152820 ,13.378930 ,7.574610 ,0.090090 ,4.236210 ,1.508270 ,1.475240 ,28.979990 ,0.000100 ,162.861300}},
 {"Y", 39 ,39 ,   88.905000 ,{17.894780 ,1.377790 ,9.911240 ,12.180840 ,7.404240 ,0.080090 ,2.144750 ,137.732300 ,1.642660 ,49.814420 ,0.000100 ,0.421870}},
 {"Y3+", 39 ,36 , 88.905000 ,{17.835940 ,1.372900 ,10.000610 ,11.942010 ,7.342990 ,0.079790 ,0.769950 ,27.591790 ,0.051610 ,0.083110 ,0.000100 ,137.725300}},
 {"Zr", 40 ,40 ,  91.220000 ,{18.008770 ,1.250420 ,10.471080 ,11.259720 ,7.222340 ,0.070500 ,2.432630 ,49.094080 ,1.864050 ,131.675100 ,0.000100 ,1.764800}},
 {"Zr4+", 40 ,36 ,91.220000 ,{17.887970 ,1.240060 ,10.578320 ,10.600350 ,7.187250 ,0.069440 ,0.347500 ,29.005430 ,0.000100 ,131.455500 ,0.000100 ,1.678290}},
 {"Nb", 41 ,41 ,  92.906000 ,{18.187220 ,1.139930 ,11.073490 ,10.826830 ,7.027860 ,0.061160 ,3.352240 ,38.717340 ,1.352500 ,115.180100 ,0.006060 ,1.195500}},
 {"Nb3+", 41 ,38 ,92.906000 ,{17.942690 ,1.139110 ,11.649380 ,10.822910 ,7.035420 ,0.061470 ,1.175710 ,34.402930 ,0.203530 ,1.158320 ,0.000100 ,134.274900}},
 {"Nb5+", 41 ,36 ,92.906000 ,{17.357130 ,1.131810 ,10.990740 ,9.522780 ,7.040500 ,0.061990 ,0.570790 ,1.113780 ,0.045420 ,134.279800 ,0.000100 ,38.407650}},
 {"Mo", 42 ,42 ,  95.940000 ,{18.360000 ,1.032910 ,6.753200 ,0.050000 ,6.254700 ,10.101350 ,5.529720 ,10.121790 ,3.767740 ,34.166930 ,1.333380 ,104.105000}},
 {"Mo3+", 42 ,39 ,95.940000 ,{16.708470 ,1.026280 ,11.989670 ,9.863980 ,6.704510 ,0.048480 ,1.985530 ,26.235840 ,1.612670 ,1.026130 ,0.000100 ,83.383880}},
 {"Mo6+", 42 ,36 ,95.940000 ,{16.846710 ,1.014890 ,11.183170 ,8.317760 ,6.671500 ,0.047720 ,1.216680 ,1.015110 ,0.083060 ,36.371420 ,0.000100 ,83.399080}},
 {"Tc", 43 ,43 ,  98.000000 ,{18.531130 ,0.931120 ,12.721350 ,9.268000 ,6.396810 ,0.037030 ,2.888110 ,31.916810 ,1.720020 ,110.118210 ,0.741480 ,44.072740}},
 {"Ru", 44 ,44 , 101.070000 ,{18.820220 ,0.843630 ,13.496360 ,8.842770 ,6.011360 ,0.023550 ,3.541020 ,27.021790 ,1.199620 ,44.092840 ,0.932070 ,113.684840}},
 {"Ru3+", 44 ,41 ,101.070000 ,{16.201210 ,0.836510 ,13.684890 ,8.666210 ,5.926930 ,0.020830 ,2.620370 ,0.836530 ,2.567510 ,22.329150 ,0.000100 ,67.416690}},
 {"Ru4+", 44 ,40 ,101.070000 ,{15.976710 ,0.834520 ,13.589210 ,8.386790 ,5.918390 ,0.020660 ,2.791820 ,0.833870 ,1.725640 ,21.207830 ,0.000100 ,67.422650}},
 {"Rh", 45 ,45 ,  102.905000 ,{19.150930 ,0.759360 ,14.438980 ,8.275230 ,4.669720 ,26.679650 ,4.662630 ,0.006940 ,1.225220 ,97.042100 ,0.851250 ,0.006950}},
 {"Rh3+", 45 ,42 ,102.905000 ,{14.552430 ,8.096000 ,14.365200 ,0.752500 ,5.431090 ,0.004220 ,3.600850 ,0.753810 ,2.865670 ,21.003250 ,1.186010 ,0.758950}},
 {"Rh4+", 45 ,41 ,102.905000 ,{14.571650 ,7.907590 ,14.109960 ,0.750120 ,5.408510 ,0.003540 ,3.657680 ,0.753380 ,1.900130 ,19.972140 ,1.354840 ,0.751240}},
 {"Pd", 46 ,46 ,  106.400000 ,{19.323000 ,0.697500 ,15.301620 ,7.931320 ,5.269700 ,0.000100 ,5.123380 ,23.541330 ,0.980210 ,60.824990 ,0.000100 ,1.282910}},
 {"Pd2+", 46 ,44 ,106.400000 ,{19.273900 ,0.695110 ,15.677870 ,7.844820 ,5.260360 ,0.000100 ,3.786850 ,22.217750 ,0.000100 ,60.823680 ,0.000100 ,1.129940}},
 {"Pd4+", 46 ,42 ,106.400000 ,{19.166080 ,0.692200 ,15.582480 ,7.509800 ,5.249910 ,0.000100 ,1.979490 ,19.350210 ,0.024520 ,0.691390 ,0.000100 ,60.830560}},
 {"Ag", 47 ,47 ,  107.870000 ,{19.283300 ,0.645190 ,16.715190 ,7.487850 ,5.184500 ,0.000100 ,4.777930 ,24.792250 ,1.038070 ,100.314050 ,0.000100 ,2.339510}},
 {"Ag+", 47 ,46 , 107.870000 ,{19.293330 ,0.645340 ,16.767860 ,7.547100 ,5.184190 ,0.000100 ,4.691460 ,23.160340 ,0.063340 ,100.325700 ,0.000100 ,2.351140}},
 {"Ag2+", 47 ,45 ,107.870000 ,{19.260380 ,0.643830 ,16.761180 ,7.442150 ,5.177280 ,0.000100 ,3.801020 ,21.245670 ,0.000100 ,100.314300 ,0.000100 ,2.439920}},
 {"Cd", 48 ,48 ,  112.400000 ,{19.223200 ,0.595420 ,17.671070 ,6.924900 ,5.078510 ,0.000100 ,4.430170 ,24.855050 ,1.595880 ,87.612220 ,0.000100 ,31.901720}},
 {"Cd2+", 48 ,46 ,112.400000 ,{19.243280 ,0.595480 ,17.816220 ,7.038220 ,5.075560 ,0.000100 ,3.865380 ,20.122380 ,0.000100 ,87.605550 ,0.000100 ,31.885840}},
 {"In", 49 ,49 ,  114.820000 ,{19.163000 ,0.548680 ,18.591700 ,6.395000 ,4.952370 ,0.000100 ,4.279940 ,26.182240 ,2.009690 ,93.701120 ,0.000100 ,8.239220}},
 {"In3+", 49 ,46 ,114.820000 ,{19.150990 ,0.558600 ,19.026640 ,6.794900 ,5.115560 ,0.003700 ,1.728460 ,25.605390 ,1.002590 ,8.230950 ,0.000100 ,93.696240}},
 {"Sn", 50 ,50 ,  118.690000 ,{19.227040 ,5.846980 ,19.098690 ,0.504210 ,4.798410 ,0.000100 ,4.373200 ,27.225710 ,2.500370 ,81.572480 ,0.000100 ,31.568140}},
 {"Sn2+", 50 ,48 ,118.690000 ,{19.145170 ,5.867760 ,19.110020 ,0.505160 ,4.807200 ,0.000100 ,4.488610 ,24.334520 ,0.250750 ,87.002220 ,0.201030 ,31.418460}},
 {"Sn4+", 50 ,46 ,118.690000 ,{19.714310 ,6.040520 ,19.145500 ,0.505060 ,4.797670 ,0.000100 ,2.346450 ,16.178280 ,0.000100 ,87.059090 ,0.000100 ,31.497910}},
 {"Sb", 51 ,51 ,  121.750000 ,{19.040770 ,0.461760 ,13.054120 ,5.319000 ,6.636700 ,5.319530 ,4.959630 ,28.541980 ,4.609410 ,0.000100 ,2.697950 ,72.651740}},
 {"Sb3+", 51 ,48 ,121.750000 ,{19.060930 ,0.463900 ,12.909280 ,5.358840 ,6.649010 ,5.358530 ,4.632780 ,0.000100 ,4.607320 ,21.751290 ,0.141400 ,70.663620}},
 {"Sb5+", 51 ,46 ,121.750000 ,{19.552740 ,5.575600 ,19.110160 ,0.464330 ,4.625850 ,0.000100 ,1.753780 ,15.085940 ,0.961700 ,5.575710 ,0.000100 ,70.668600}},
 {"Te", 52 ,52 ,  127.600000 ,{19.963270 ,4.818790 ,18.996860 ,0.421690 ,6.191480 ,28.798580 ,4.385830 ,0.000100 ,2.461940 ,70.638640 ,0.000100 ,12.770960}},
 {"I", 53 ,53 ,   126.904000 ,{18.979250 ,0.382600 ,15.695780 ,4.348790 ,7.064330 ,26.936040 ,4.424890 ,4.352100 ,4.100180 ,0.000100 ,2.732710 ,61.598360}},
 {"I-", 53 ,54 ,  126.904000 ,{18.975340 ,0.381650 ,15.688410 ,4.332170 ,6.747140 ,26.511280 ,4.421940 ,4.350070 ,4.084310 ,0.000130 ,4.068540 ,70.735290}},
 {"Xe", 54 ,54 ,  131.300000 ,{20.297870 ,3.938380 ,19.005560 ,0.345880 ,9.041650 ,26.700660 ,3.760220 ,0.000100 ,1.895610 ,65.344760 ,0.000100 ,20.303050}},
 {"Cs+", 55 ,54 , 132.905000 ,{17.287150 ,0.333020 ,15.874580 ,3.470490 ,10.366780 ,24.265960 ,5.188490 ,0.047820 ,4.518210 ,3.990220 ,0.763140 ,61.277240}}
};//formfactors
const double pisqrt=sqrt(M_PI);
const double pi2=M_PI*M_PI;
const double twopi2=2.0*pi2;
const double pisqrt3=M_PI*pisqrt;
const double acc=1e-6;
class Hirshfeld: public QDialog{
  Q_OBJECT
public:
  Hirshfeld(molekul *externMole,QList<INP> *_au,QList<INP> *sel,QString *_dirName);
  QList<HiAtom> atomsInCell, atomsInMolecule;
  molekul *mol;
  QList<INP> *au;
  QList<INP> *selectedAtoms;
  QString *dirName;
  Cell expandedCell;
  void forMolIso(MolIso *mi);
  int ntot,nx,ny,nz;
  int entot,enx,eny,enz;
  double expansion;
private:
  Matrix u2b(const Matrix u);
  double *iamr;
  double *crys_rho;
  double *mole_rho;
  double *atom_rho;
  double *hirsh_share;
  double *C,*_C;
  double *expandetrho2;
  double *density_out;
  QSpinBox *nxs, *nys, *nzs;
  QLabel *xSpacing, *ySpacing, *zSpacing;
  QCheckBox *staticBx;
  QProgressBar *forts;
  QLabel *aStatusBar;
  QString IAMpriorname;
  QString IAMmolname;
  QString DensityIn ;
  QString DensityOut;
  QString HirshShare;
  QLineEdit *iampre;
  QLineEdit *iammol ;
  QLineEdit *densin ;
  QLineEdit *densout;
  QLineEdit *hirshsh;
  QPushButton *iamp,*loadIamp; 
  QPushButton *hirp;
  Vector3 center;
  QString selectedFilter,sfacstr,isfac;
  QList<int> magic;
  void prepareMagicNumbers();
  int findSFAC(int an);
  void fillUnitCell(QList<INP> xdinp);
  void fillmi(HiAtom &ha);
  double calcRho(HiAtom ha, int ix, int iy, int iz);
  double checkSymmOfMap();
  void applySymmOnMap();
  void setup_zelle(); 
  void frac2kart (V3 x, V3 & y); 
  void hirsh41atom(const INP,double *ra,double *rc, double *dd);
  void readCube(double *dat, QString fileName);
  void readRaw(double *dat,  QString fileName);
  void readM81(double *dat,  QString fileName);
  void writeCube(double *data, QString fileName);
  void writeCube2(double *data, QString fileName);
  void writeRaw(double *data, QString fileName);
  void writeRaw2(double *data, QString fileName);
  void writeM81(double *data, QString fileName);
  void writeM812(double *data, QString fileName);
public slots:
 
  void changeExpansion(double d); 
  void priorThere(const QString & text);
  void hirshThere(const QString & text);
  void stickyNumbersx(int i);
  void stickyNumbersy(int i);
  void stickyNumbersz(int i);
  void statdyn();
  void iamprior();
  void loadIAMPrior();
  void expandBox();
  void browseIAMpriorname();
  void browseIAMmolname();
  void browseDensityIn ();
  void browseDensityOut();
  void browseHirshShare();
  void handleIAMpriorname();
  void handleIAMmolname();
  void handleDensityIn ();
  void handleDensityOut();
  void handleHirshShare();
  void writeMap(double *dat, QString fileName);
  void writeMap2(double *dat, QString fileName);
  void readMap(double *dat, QString fileName);
signals:
  void doface();
};//class
