t <html><head><title>Alarm Control</title></head>
i pg_header.inc
t <h2 align=center><br>Alarm Control</h2>
t <p><font size="2">This page allows you to change a message displayed on <b>LCD Module</b>.
t  Simply enter the text which you want to display and click <b>Send</b>.<br><br>
t  This Form uses a <b>POST</b> method to send data to a Web server.</font></p>
t <form action=alarm.cgi method=post name=cgi>
t <input type=hidden value="alarm" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Hours</td>
c j 1 <td><input type=text name=hours size=20 maxlength=2 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Minutes</TD>
c j 2 <td><input type=text name=minutes size=20 maxlength=2 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Seconds</TD>
c j 3 <td><input type=text name=seconds size=20 maxlength=2 value="%s"></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t <input type=submit name=set value="Set" id="sbm">
t <input type=reset value="Undo">
t </p></form>
i pg_footer.inc
. End of script must be closed with period.
