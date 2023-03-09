t <html><head><title>Time information</title>
t <script language=JavaScript type="text/javascript" src="xml_http.js"></script>
t <script language=JavaScript type="text/javascript">
# Define URL and refresh timeout
t var formUpdate = new periodicObj("ad.cgx", 500);
t var formUpdate = new periodicObj("ad.cgx", 500);
t function plotADGraph() {
t  numVal = parseInt(adVal, 16);
t  voltsVal = (numVal*3.3/4096);
t  tableSize = (numVal*100/4096);
t  document.getElementById("ad_table").style.width = (tableSize + '%');
t  document.getElementById("ad_volts").value = (voltsVal.toFixed(3) + ' V');
t }
t function periodicUpdateAd() {
t   {
t   updateMultiple(formUpdate,showtime);
t   ad_elTime = setTimeout(periodicUpdateAd, formUpdate.period);
t  }
t  else
t   clearTimeout(ad_elTime);
t }
t </script></head>
i pg_header.inc
t <h2 align="center"><br>Time information</h2>
t <p><font size="2">This page allows you to see the time and date.</p>
t <form action="time.cgi" method="post" name="ad">
t <input type="hidden" value="ad" name="pg">
t <table border=0 width=99%><font size="3">
t <tr style="background-color: #aaccff">
t  <th width=50%>Time</th>
t  <th width=50%>Date</th>
c h	1 <tr><td>%s
t   <td align="center">
c h	2 %s</td></tr>
t </font></table>
#Refresh page each second
t <META HTTP-EQUIV="REFRESH" CONTENT="1">
t </p></form>
i pg_footer.inc
. End of script must be closed with period
