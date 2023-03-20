t <html><head><title>RTC Control</title>
t <script type=text/javascript>
t setTimeout("document.location=document.location",1000);
t </script>
t </head>
i pg_header.inc
t <h2 align=center><br>HORA Module Control</h2>
t <p><font size="2">En este apartado vamos a visualizar la fecha y hora del sistema.<br><br>
t <form action=hora.cgi method=post name=cgi>
t <input type=hidden value="lcd" name=pg>
t <table border=0 width=99%><font size="3">
t <tr bgcolor=#aaccff>
t  <th width=40%>Item</th>
t  <th width=60%>Setting</th></tr>
# Here begin data setting which is formatted in HTTP_CGI.C module
t <tr><td><img src=pabb.gif>Hora:</td>
c z 1 <td><input type=text name=lcd1 size=20 maxlength=20 value="%s"></td></tr>
t <tr><td><img src=pabb.gif>Fecha:</td>
c z 2 <td><input type=text name=lcd2 size=20 maxlength=20 value="%s"></td></tr>
t </font></table>
# Here begin button definitions
t <p align=center>
t </p></form>
i pg_footer.inc
. End of script must be closed with period.