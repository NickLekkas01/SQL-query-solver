Ανάπτυξη Λογισμικού Για Πληροφοριακά Συστήματα - Μέρος 2ο


Μέρος 1 - Αποθήκευση Relation

Δομή Relation: Num Tuples( αριθμός του μεγέθους του Tuple )
 Num Rows ( αριθμός των γραμμών στο αρχείο )
 RelSerialData* ( σειριακά αποθηκευμένα data του Relation )

Διατηρούμε την σειριακή αποθήκευση που έχουν τα binary αρχεία, αποθηκεύουμε σε έναν πίνακα Tuples*Rows.


Μέρος 2 - Επεξεργασία Query

Batch : Λίστα από Queries. 

Κάθε στοιχείο του batch περνάει στον query Executor.

Query Data: 
Bindings : πίνακας από δείκτες σε Relations.
Predicates: Πίνακας από strings. 
Projections: Πίνακας από ζευγάρια ints

Αποθήκευση σε Intermediate Results: 


Για βελτιστοποίηση, έχουμε αποθηκεύσει με διαφορετικό τρόπο τα φίλτρα από τις join.

Αποθήκευση φίλτρου: 
Για κάθε relation που λαμβάνει μέρος στο query (binding), υπάρχει η δυνατότητα να αποθηκευτεί ένας μονοδιάστατος πίνακας από RowIds. 

Αποθήκευση join: 
Αποθηκεύουμε τις πλειάδες όμοια με τον τρόπο που αποθηκεύεται το input. Σειριακά, και κρατάμε το μέγεθος τις πλειάδας αναλόγως με το πώς αλλάζει όταν εισάγουμε νέα αποτελέσματα. 


Υποπερίπτωση Join: μεταξύ ίδιας Relation.
Τότε συμπεριφερόμαστε σαν να είναι τα αποτελέσματα της join φίλτρο και τα αποθηκεύουμε στη θέση των φίλτρων. 



Χρήση Intermediate Results: 

Έλεγχος 1: Αν υπάρχει η σχέση του Predicate μέσα στα αποτελέσματα της join, τότε θα χρησιμοποιήσουμε τα εκεί αποθηκευμένα RowIds.

Έλεγχος 2: Αν ο Έλεγχος 1 αποτύχει, τότε αν υπάρχει η σχέση στα φίλτρα, θα χρησιμοποιηθούν εκείνα τα RowIds.

Έλεγχος 3: Αν αποτύχουν οι παραπάνω έλεγχοι, πάρε τα δεδομένα από τα Bindings. 
