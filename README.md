<b>#Rosu Mihai Cosmin 313CA</b>

In rezolvarea temei am folosit implementarile facute la laborator pentru
hashtable si pentru liste simplu inlantuite.


<b><i>SERVER_MEMORY:</i></b>  
-Am implementat server_memory-ul cu un hashtable.  
-Pentru functiile de comparare si de hash necesare hashtable-ului  
am folosit functiile din cadrul laboratorului.  

<b>init_server_memory:</b>  
-Am alocat memorie pentru server_memory dupa care am creat hashtable-ul din  
cadrul acestuia. In final returnez server_memory-ul alocat mai devreme.  

<b>server_store:</b>  
-Pentru a stoca elementul si cheia primite, apelez functia ht_put pentru  
a le adauga in hashtable. Functia determina bucket-ul in care va fi pusa  
perechea cheie-valoare folosindu-se de functia de hash.  

<b>server_remove:</b>  
-Pentru a elimina elementul cu cheia primita din hashtable, apelez functia  
ht_remove_entry care cauta in hashtable elementul cu cheia primita si le  
elimina, eliberand memoria acestora.  

<b>server_retrieve:</b>  
-Pentru a prelua elementul cu cheia precizata, mai intai verific daca exista  
aceasta cheie in hashtable.  
-Daca exista, apelez functia ht_get care preia valoarea din perechea  
cheie-valoare unde cheia este cea primita, si o returnez.  
-Daca nu exista returnez NULL.  

<b>free_server_memory:</b>  
-Eliberez memoria hashtable-ului si a server_memory-ului.  


<b><i>LOAD_BALANCER:</i></b>  
-Am retinut in structura load_balancer-ului: un hash_ring, reprezentat  
printr-o lista simplu inlantuita care retine perechi de server_id-hash pentru  
fiecare replica de server; un vector de server_memory-uri cu 100000 de  
elemente, astfel incat pe pozitia i din vector este server_memory-ul celui  
de-al i-ulea server; si un server_count.  

<b>init_load_balancer:</b>  
-Aloc memorie pentru load_balancer folosind calloc, deoarece initializeaza  
vectorul de server_memory-uri cu NULL.  
-Creez lista hash_ring-ului cu data_size-ul unui pair_id_hash_t, care retine  
un int (id-ul server-ului) si un unsigned int (hash-ul server-ului).  
-Initializez server_count-ul cu 0 si returnez load_balancer-ul alocat.  

<b>loader_store:</b>  
-Retin hash-ul cheii primite si caut in hash_ring pozitia pe care s-ar afla  
acest hash.  
-Pe pozitia gasita se afla replica server-ului pe care trebuie adaugat noul  
obiect.  
-Daca pozitia gasita este dupa ultimul element din hash_ring, inseamna ca  
obiectul va fi adaugat pe primul server din hash_ring.  
-In final apelez functia server_store pentru server-ul determinat.  

<b>loader_retrieve:</b>  
-Parcurg primii trei pasi din loader_store pentru a determina server-ul de pe  
care trebuie preluat obiectul.  
-Dupa ce a fost gasit returnez rezultatul apelului functiei server_retrieve  
pentru server-ul determinat.  

<b>loader_add_server:</b>  
-In implementarea acestei functii am folosit functia hash_ring_add_server care  
adauga o replica a unui server pe hash_ring in functie de hash-ul server-ului.  
-Daca doua servere au acelasi hash, ele vor fi ordonate in functie de id.  
-Dupa ce este adaugata o replica functia hash_ring_add_server returneaza  
pozitia pe care a fost adaugata replica.  
-Folosind aceasta pozitie determin replica precedenta (prev) de pe hash_ring  
si replica urmatoare (next).  
-Apoi parcurg elementele din hashtable-ul server-ului din next si pentru  
fiecare recalculez hash-ul si verific daca respecta conditia de preluare a  
load-ului de catre noul server. In acest caz adaug elementul in noul server si  
il elimin din next.  
-Aceasta conditie depinde de pozitia pe care a fost adaugata replica noului  
server.  
-Daca pozitia este 0 conditia este ca hash-ul elementului sa fie mai mare  
decat prev (adica mai mare decat orice hash-ul oricarui server, prev fiind  
ultima replica din hash_ring) si mai mic decat hash-ul replicii curente (adica  
mai mica decat hash-ul oricarui alt server, intrucat noua replica a fost  
adaugata pe pozitia 0).  
-Daca pozitia este diferita de 0, hash-ul elementului trebuie sa fie intre  
hash-ul lui prev si hash-ul curent.  

<b>loader_remove_server:</b>  
-In implementarea acestei functii am folosit functia hash_ring_remove_server  
care elimina toate replicile de pe hash_ring ale unui server.  
-Dupa ce am eliminat replicile server-ului ce trebuie scos, ii parcurg  
hashtable-ul si pentru fiecare element apelez functia loader_store pentru a  
le stoca pe un alt server, si le elimin din server-ul ce trebuie scos.  
-In final eliberez memoria server_memory-ului, il setez pe NULL si decrementez  
server_count-ul.  

<b>free_load_balancer:</b>  
-Folosind un for parcurg tot vectorul de server_memory-uri, iar pentru fiecare  
server_memory ce este diferit de NULL (adica a fost alocat) ii eliberez  
memoria. Apoi eliberez memoria hash_ring-ului si a load_balancer-ului.  
