===================
The API to GET or POST the data is formatted like -  
http://localhost:8080/CM/api/financial/kpi/profitability
===================
Account Info
cloudmetrics.fintech@gmail.com
===================
AWS instance
scp -i ~/Dropbox/amazonkey/cloudmetrics.pem D:/work/shippable/CloudMetrics/CloudMetrics-web.war ubuntu@54.149.129.163:~/.
ssh -i ~/Dropbox/amazonkey/cloudmetrics.pem 54.149.129.163 -l ubuntu

sudo chmod 777 /var/cm_data/ -R

Installing mysql and tomcat7
sudo apt-get install mysql-server tomcat7

Start and stop servers using
sudo /etc/init.d/tomcat7 start|stop
sudo /etc/init.d/mysql start|stop

tomcat deployment dir is
/var/lib/tomcat7

Config files can be found in
/etc/mysql
/etc/tomcat7
/etc/default/tomcat7.

By default only root processes can listen to portnumber < 1024. So modify IP Tables to forward 8080 to 80.
sudo /sbin/iptables -t nat -I PREROUTING -p tcp --dport 80 -j REDIRECT --to-port 8080

Include the proxyPort attribute in your HTTP connector config in server.xml:
<Connector port="8080" proxyPort="80" .../>