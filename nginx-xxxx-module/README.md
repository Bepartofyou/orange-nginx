# nginx http demo module

## Build
* git clone https://github.com/nginx/nginx.git
* git clone https://github.com/Bepartofyou/orange-nginx.git
* mkdir build && cd nginx && ./auto/configure --prefix=../build --with-debug --add-module=../orange-nginx/nginx-xxxx-module 
* make && make install

## Edit config and start nginx
* mv ../build ./objs
* vi ./objs/build/conf/nginx.conf

		listen       8080;

		location /test {
			xxxx "I am Bepartofyou, an av streaming engineer!\n";
		}
* cd ./objs && ./nginx -p ./build -c conf/nginx.conf

## Test
* curl -i http://localhost:8080/test  or  open the url(http://ip:8080/test) in your browser
