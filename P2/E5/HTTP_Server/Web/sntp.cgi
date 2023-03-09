t <html><head><title>SNTP Server Config</title>
t <script language=JavaScript>
t function AllSW(st) {
t  for(i=0;i<document.form1.length;i++) {
t   if(document.form1.elements[i].type=="checkbox"){
t    document.form1.elements[i].checked=st;
t   }
t  }
t  document.form1.submit();
t }
t </script></head>
i pg_header.inc
t <h2 align=center><br>Control LEDs on the board</h2>
t <p><font size="2">Here you can configure the SNTP server to be used:<br><br>
t Server 1:<br><br>
t Server 2:<br><br></font></p>
t <form action=leds.cgi method=post name=form1>
t <input type=hidden value="led" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%></th>
t  <th width=60%>Server select</th></tr>
t <td><img src=pabb.gif>SNTP Server:</td>
t <td><select name="ctrl" onchange="submit();">
c b c <option %s>Server 1</option><option %s>Server 2</option></select></td></tr>
t <td><table><tr valign="middle">
i pg_footer.inc
. End of script must be closed with period.


