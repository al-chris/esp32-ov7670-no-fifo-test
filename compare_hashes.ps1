           ='libraries\\OV7670\\src'
           ='libraries\\ESP32-OV7670-no-FIFO\\src'
=Get-ChildItem             -File | ForEach-Object { [PSCustomObject]@{Name=.Name; Hash=(Get-FileHash .FullName -Algorithm SHA256).Hash} }
=Get-ChildItem             -File | ForEach-Object { [PSCustomObject]@{Name=.Name; Hash=(Get-FileHash .FullName -Algorithm SHA256).Hash} }
BMP.h DMABuffer.cpp DMABuffer.h I2C.cpp I2C.h I2SCamera.cpp I2SCamera.h Log.h OV7670.cpp OV7670.h XClk.cpp XClk.h=(.Name + .Name) | Sort-Object -Unique
=False
foreach(XClk.h in BMP.h DMABuffer.cpp DMABuffer.h I2C.cpp I2C.h I2SCamera.cpp I2SCamera.h Log.h OV7670.cpp OV7670.h XClk.cpp XClk.h){
  @{Name=XClk.h; Hash=B011C471D323148266C35C4C2B01D80672A7D1809E8898B02C2EC12C6D90C1FB; Path=C:\Users\CHRISTOPHER\Documents\Arduino\esp32-ov7670-no-fifo\libraries\OV7670\src\XClk.h}= | Where-Object { .Name -eq XClk.h }
  @{Name=XClk.h; Hash=B011C471D323148266C35C4C2B01D80672A7D1809E8898B02C2EC12C6D90C1FB; Path=C:\Users\CHRISTOPHER\Documents\Arduino\esp32-ov7670-no-fifo\libraries\ESP32-OV7670-no-FIFO\src\XClk.h}= | Where-Object { .Name -eq XClk.h }
  if(-not @{Name=XClk.h; Hash=B011C471D323148266C35C4C2B01D80672A7D1809E8898B02C2EC12C6D90C1FB; Path=C:\Users\CHRISTOPHER\Documents\Arduino\esp32-ov7670-no-fifo\libraries\OV7670\src\XClk.h}){ Write-Output  ONLY_IN_ESP: XClk.h; =True }
  elseif(-not @{Name=XClk.h; Hash=B011C471D323148266C35C4C2B01D80672A7D1809E8898B02C2EC12C6D90C1FB; Path=C:\Users\CHRISTOPHER\Documents\Arduino\esp32-ov7670-no-fifo\libraries\ESP32-OV7670-no-FIFO\src\XClk.h}){ Write-Output ONLY_IN_OV: XClk.h; =True }
  elseif(@{Name=XClk.h; Hash=B011C471D323148266C35C4C2B01D80672A7D1809E8898B02C2EC12C6D90C1FB; Path=C:\Users\CHRISTOPHER\Documents\Arduino\esp32-ov7670-no-fifo\libraries\OV7670\src\XClk.h}.Hash -ne @{Name=XClk.h; Hash=B011C471D323148266C35C4C2B01D80672A7D1809E8898B02C2EC12C6D90C1FB; Path=C:\Users\CHRISTOPHER\Documents\Arduino\esp32-ov7670-no-fifo\libraries\ESP32-OV7670-no-FIFO\src\XClk.h}.Hash){ Write-Output DIFF: XClk.h; =True }
}
if(-not ){ Write-Output 'IDENTICAL' }
