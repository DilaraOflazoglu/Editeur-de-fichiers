# Editeur-de-fichiers

# INTRODUCTION
Distributed and Cooperative File Editor is a file editor that will help different users to open, read and modify a file that is stored on server, in real time.
Language C. UI : GTK API.


# BUILDING
	-	git clone https://github.com/DilaraOflazoglu/Editeur-de-fichiers.git
	-	cd Editeur-de-fichiers/src <br />
	
	
# Server Code
	-	cd server <br />
Launch the server:  <br />
	-	make test-server 
	-	OR make THEN ./bin/server Socket_Port_1 Socket_Port_2, Socket Port numbers must be greater than 1024)  <br />

# Client Code
	-	cd client <br />
Launch the client:  <br />
	-	make test-client <br />
	-	OR make THEN ./bin/client Socket_Port_1 Socket_Port_2, Socket Port numbers must be greater than 1024)  <br /> <br />
	
NOTE : CLIENT Socket_Port_1 (or 2) and SERVER Socket_Port_1 (or 2) must be the same. <br />

# FEATURES
	
