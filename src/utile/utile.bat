set PATH=%XSCHEM_SHAREDIR%/utile;%PATH%
set UTILE3_PATH="%XSCHEM_SHAREDIR%/utile
@ECHO OFF 
preprocess.awk "%*" | expand_alias.awk | param.awk | clock.awk | stimuli.awk