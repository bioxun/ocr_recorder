
MaxCharNum=3000
user_defined_dpi=200

-- ------------------------------------------
--whitelist=""
--blacklist="a "
-- ------------------------------------------
engine_oem=1
-- ------------------------------------------
PageSegMode=4
PageSegMode=6
--PageSegMode=7

-- ------------------------------------------
target_loc={451,835,915,32}

--------------------------------------------
modeScript=0

--------------------------------------------
doDilate=0
r_near=0.4
--------------------------------------------
doLab=1
--------------------------------------------
Rrc_seekColor={0.375,0.25,0.25,0.5}
--Rrc_seekColor={0.3,0.1,0.4,0.8}
--------------------------------------------
doDebug=1
doShow=1

--------------------------------------------
alpha_contrast=1
beta_contrast=10
blockSize1=5    -- 重要
doBlur=1
canny_thresh1=85
canny_thresh2=185
--------------------------------------------
doFlood1=1
doFlood2=doFlood1
colorFloodB={255,255,255}
flood_fillMode=4
flood_connect=4
N_floodPointX=90
N_floodPointY=10
colorFlood_lowDiff=1
colorFlood_upDiff=1
--------------------------------------------
min_area=0
--------------------------------------------
colorTarget={255,255,255,30}

-- ------------------------------------------
auto_colorTarget=1
dist_colorTarget=20
-- ------------------------------------------
preprocessType=3

-- ------------------------------------------
Is_DarkChar=0
imgFile="Images/example08.png"

--------------------------------------------
tmp=1
--------------------------------------------
ConfigFile="ocr_tesser.config"
--ConfigFile=""

-- ------------------------------------------
--lang="eng"    -- language
lang="chi_sim"    -- language

-- ------------------------------------------
--path_tessdata="D:/ProgKits/msys2/mingw64/share/tessdata"   -- fold with .traindata
path_tessdata = "G:/softwares_program/0_C/packages/tesseract/tessdata_best"

