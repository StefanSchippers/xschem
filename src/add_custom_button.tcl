## This example shows how to add a custom toolbar button to xschem
## Stefan Schippers 20220827

## 24x24 GIF image (so it will be loaded by 20 years old tcl8.4, recent
## tcl accepts also PNG). Transparent background, encoded as base64

set topwin [xschem get top_path]
## Add a "MyButton" button to toolbar_list

if {[lsearch -exact $toolbar_list MyButton] < 0} {
  lappend toolbar_list MyButton
  set MyButtonData {
    R0lGODlhGAAYAPcAACIiIiMjIyUlJSYmJicnJykpKSoqKisrKywsLDAwMDExMTIyMjMzMzQ0NDU1
    NTY2Njc3Nzg4ODk5OTo6Oj09PT4+Pj8/P0FBQUJCQkNDQ0REREVFRUZGRklJSUxMTE1NTU5OTlFR
    UVJSUlNTU1lZWVpaWltbW1xcXF1dXV5eXl9fX2FhYWNjY2RkZGZmZmdnZ2hoaGlpaWtra2xsbG1t
    bXFxcXJycnR0dHV1dXl5eXt7e3x8fICAgIODg4eHh4uLi4yMjI2NjY6Ojo+Pj5GRkZKSkpSUlJaW
    lpeXl5mZmZqampubm5ycnJ2dnZ6enp+fn6GhoaKioqOjo6SkpKampqenp6ioqKmpqa6urrCwsLGx
    sbOzs7S0tLW1tba2tre3t7m5ubu7u76+vsDAwMHBwcLCwsbGxsfHx8jIyMnJycrKysvLy83Nzc/P
    z9DQ0NLS0tPT09XV1dbW1tfX19jY2NnZ2dra2tvb29/f3+Hh4eLi4uXl5efn5+jo6Onp6erq6uvr
    6+zs7O/v7/Dw8PX19fb29vf39/j4+Pn5+fr6+vv7+/z8/P39/f7+/gAAAAAAAAAAAAAAAAAAAAAA
    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAAI4ALAAAAAAYABgA
    AAj+AB0JHDhwUR85ew4RXMhQoBgaCABIFJDiisKGBPGUkAhAAAQDHDGUwShQjQIACH68uahniYSO
    UTDaiViiT8NCOyRiYXgIAwAXZlKMoDJQjdArRwAYwLOwCQANdkBKzOIIj1QAXGgAkEFwEQQAYZyy
    CAKAhaMlP39s3QPSzsA2ADAsogIAhAwANBxFqXu3hiMcAI4MJAJghyNBEgR0VHP4Zcc2jriUHZiT
    qKM+QXi8Gdjnxw45Au08HaiVC0mGfQAooAzA8umBojUMBsDjNcEwAFIUHe2IRo2LBJuMsJkjcMGT
    ZRadlDGoIGEAcgRFBD0waYdCaiIiwHGERwWJTf5TTiZY6HuNRXJAcJQIIYzejtQJvgHpYtAiNUFy
    BOFyaBHZ1hiVEZECR+gxkCBXaNCRYCSlx5ECGnwlkQSmvbZIFiwoxhEISxRiG3l2mBHdhyQ2FBAA
    Ow==
  }

  ## Create an image object. Name should be img<name of button>
  image create photo imgMyButton 
  imgMyButton put $MyButtonData

}

## Create the toolbar button
## constructor  name         tcl command        tooltip    topwindow
toolbar_add   MyButton      { puts SMILE! }     "SMILE!"     $topwin
## Destroy and rebuild the toolbar, but see better option here under ...
# toolbar_hide
# toolbar_show

## ... More efficient way: just add the new button ...
pack $topwin.toolbar.bMyButton -side left

## ... or place it before some other button:
# pack $topwin.toolbar.bMyButton -after $topwin.toolbar.bViewToggleColors -side left

## To remove the button without destroying it:
# pack forget $topwin.toolbar.bMyButton
