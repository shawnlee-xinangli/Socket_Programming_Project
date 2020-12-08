# Socket_Programming_Project

# What I have done in the assignment?
 
      Built TCP connection between clients and the mainserver and UDP connection between mainserver and the backend servers.
      Give the recommendation for the client's input based on the recommendation algorithm.

# What my code files are and what each one of them does? 

      serverA/B.cpp: UDP socket is created with the mainserver. Read data from the input file and build graph. Implement the recommendation algorithm using priority_queue.

      servermain.cpp: TCP socketes are created with clients, which handles concurrent queries. UDP sockets are created with backend servers.

      client.cpp: TCP socket is created with the mainserver, which handles user input and received message from the mainserver.

# The format of all the messages exchanged

          In serverA/B.cpp: 
                Receive:                 
                    Case1: 0 countryList;                    Case2: 1 + country_name + userId
                Send:
                    Case1: country1 country2 ... countryN         Case2: Recommendation Result    

           In servermain.cpp: 
                UDP:
                   Send:       
                       Case1: 0 countryList;                Case2: 1 + country_name + userId 
                   Receive:
                       Case1: country1 country2 ... countryN   Case2: Recommendation Result
                TCP:
                    Receive: country_name + userId
                    Send: Result from the backend                                         

           In client.cpp:
                      Query: country_name + userId
                      Response: Result from the backend

# Idiosyncrasy of my project

      We assumed the number of users for each country is no greater than 100. Thus, my code can only handle the case that there are no more than 100 users in each country.
      We assumed the country names are all letters(a-z, A-Z) and the userIds are all numbers(0-9). But I still handled the invalid input cases.

# Reused Code

      I used sample code from "Beej's Guide to Network Programming" as template.
      UDP sample code from "https://www.jianshu.com/p/6c065429d3c1"
      TCP sample code from "https://www.jianshu.com/p/3b233facd6bb"
