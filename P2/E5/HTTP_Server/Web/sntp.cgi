t <html><head><title>SNTP Server Config</title>
t <script language=JavaScript>
t </script></head>
i pg_header.inc
t <h2 align=center><br>Select SNTP server</h2>
t <p><font size="2">Here you can configure the SNTP server to be used:<br><br>
t Server 1: hora.rediris.es (130.206.3.166)<br><br>
t Server 2: hora.roa.es (150.214.94.5)<br><br></font></p>
t <form action=sntp.cgi method=post name=form1>
t <input type=hidden value="led" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#fcce4e>
t  <th width=40%></th>
t  <th width=60%>Server select</th></tr>
t <td><img src=pabb.gif>SNTP Server:</td>
t <td><select name="sntp" onchange="submit();">
c i c <option %s>Server 1</option><option %s>Server 2</option></select></td></tr>
t <td><table><tr valign="middle">
i pg_footer.inc
. End of script must be closed with period.


