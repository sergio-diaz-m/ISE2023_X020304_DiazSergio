t <html><head><title>Time</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("time.cgx", 100);
t function plotTime() {
t  timeVal = document.getElementById("time_value").value;
t }
t function plotDate() {
t  dateVal = document.getElementById("date_value").value;
t }
t function periodicUpdate() {
t   updateMultiple(formUpdate,plotTime);
t   updateMultiple(formUpdate,plotDate);
t   setTimeout(periodicUpdate, formUpdate.period);
t }
t periodicUpdate();
t </script></head>
i pg_header.inc
t <h2 align="center"><br>NTP Server</h2>
t <p><font size="2">This page allows you to visualize the current date and time
t  from the system. Periodic screen update is based on <b>xml</b> technology.</font></p>
t <form action="time.cgi" method="post" name="ad">
t <input type="hidden" value="time" name="pg">
t <table border=0 width=99%><font size="3">
# Color de recuadros
t <tr style="background-color: #f79292">
t  <th width=50%>Time</th>
t  <th width=50%>Date</th></tr>
t <tr>
t   <td align="center">
t <input type="text" readonly style="background-color: transparent; border: 0px"
c h 1 size="10" id="time_value" value="%s"></td>
t <td align="center"><input type="text" readonly style="background-color: transparent; border: 0px"
c h 2 size="10" id="date_value" value="%s"></td>
t </font></table>
t <p align=center>
t </p></form>
# t <META HTTP-EQUIV="REFRESH" CONTENT="1">
i pg_footer.inc
. End of script must be closed with period
