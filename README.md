# Editeur-de-fichiers

# INTRODUCTION
Distributed and Cooperative File Editor is a file editor that will help different users to open, read and modify a file that is stored on server, in real time.
Language C. UI : GTK API.<br />


# BUILDING
	-	git clone https://github.com/DilaraOflazoglu/Editeur-de-fichiers.git
	-	cd Editeur-de-fichiers/src
	
	
# Server Code
	-	cd server
Launch the server:  <br />
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp;make test-server <br />
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp;OR make THEN ./bin/server Socket_Port_1 Socket_Port_2 <br />

# Client Code
	-	cd client
Launch the client:  <br />
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp;make test-client <br />
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp;OR make THEN ./bin/client Socket_Port_1 Socket_Port_2 <br /> <br />
	
NOTE 1 : CLIENT Socket_Port_1 and SERVER Socket_Port_1 must be the same. <br />
NOTE 2 : CLIENT Socket_Port_2 and SERVER Socket_Port_2 must be the same. <br /> 
NOTE 3 : Socket Port numbers must be greater than 1024 <br />


# FEATURES
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp; Chose a file provided by the server and Open it on User Interface <br />
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp; Edit this file <br />
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp; Get all other Users who opened this file in Real-Time <br />
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp; One user can edit one line at the same time <br />
&nbsp; &nbsp; &nbsp;-&nbsp; &nbsp; &nbsp; Different Users can not edit the same line, to respect the consistency <br />
	
	
# USER INTERFACE
![alt text](images/file_editor_design.png)
