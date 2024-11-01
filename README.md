# Mini solveur de PPC

## Table of Contents
- [Création des Variables](#variables)
- [Création des contraintes](#contraintes)
- [Création d'une instance](#instance)
- [Utilisation du solveur](#solver_usage)

## Création des variables 

Les variables sont représentées par : 
- un nom
- un indice, qui correspond à leur place dans le vecteur des variables définissant le CSP 
- un domaine, qui est représenté comme un vecteur d'entiers que peut prendre la variable

Afin de créer une variable, on fait appel au constructeur de la classe de la manière suivante : `Variable(string name, int index, vector<int> domain)`

## Création des contraintes

Les contraintes peuvent être représentées sous deux formes différentes, dépendant de leur type. Les classes sont alors des enfants de la classe mère. Afin de créer une contrainte de tuple, il suffit de faire appel au constructeur de la classe de la manière suivante : `Constraint`

### Les contraintes de tuples

Il s'agit de contraintes binaires "classiques", c'est-à-dire modélisant la sitation $(x,y) \in \{(a_1, b_1), ..., (a_n, b_n) \}$. Elles sont représentées par : 
- un couple de variables, qui sont référencées par leurs indices
- un vecteur de couples de valeurs, que les variables peuvent prendre simultanément

Afin de créer une contrainte de tuple, il suffit de faire appel au constructeur de la classe de la manière suivante : `Tuple_Constraint(pair<int, int> vars, vector<tuple<int, int>> admissible_tuples)`


### Les contraintes de différences
Il s'agit des contraintes de la forme $ax+by \ne c$, elles sont représentées par : 
- un couple de variables, qui sont référencées par leurs indices 
- un couple de coefficients (a,b) 
- une valeur c définissant la valeur interdite
Afin de créer une contrainte de différence, il suffit de faire appel au constructeur de la classe de la manière suivante : `Diff_Constraint(pair<int, int> vars, pair<int, int> coeffs, int forbiden_nb)`


## Création d'une instance

Afin de créer une instance de CSP, il suffit alors de créer : 
- un vecteur de contraintes : Elles peuvent être de types différents
- un vecteur de variables: Il faut faire attention à ce que leurs indices soient uniques et qu'ils correspondent à leur position dans le vecteur

Afin de créer un CSP, il suffit de faire appel au constructeur de la classe de la manière suivante : `CSP(vector<Constraint> constraints, vector<Variable> variables)`

## Utilisation du solveur

Le solveur mis à disposition est fait pour être testé sur trois type de problèmes différents : 
- Le problème des n_reines, à l'aide des fonctions `create_n_queens_CSP_tuples` (avec des contraintes de tuples) et `create_n_queens_CSP_diff` (avec des contraintes de différences). Ces fonctions prennent en entrée le nombre de reines désiré et renvoient le CSP correspondant.
- Le problème du sudoku à l'aide des fonctions `create_sudokus_CSP_tuples` et `create_sudokus_CSP_diff` qui prennent en entrée le nom du fichier `.txt` correpondant à l'instance de sudoku souhaitée sous la forme d'une grille de sudoku où les cases vides sont représentées par des 0.
- Le problème de coloration à l'aide de la fonction `create_graph_coloring_CSP` qui prend en argument le nom du fichier `.col` correpondant à l'instance de coloration souhaitée, où chaque ligne est de la forme `e idx_i idx_j` indiquant que les noeuds i et j sont voisins. 

Une fois le CSP créé à l'aide de ces différentes fonctions, il s'agit de le résoudre à l'aide de la méthode `solve` de la class `CSP`. Cette fonction prend un certain nombre d'arguments qui permettent notamment d'activer ou de désactiver certaines fonctionnalités du solveur (comme la consistance ou autre).
- le dictionnaire d'assignation des variables de la forme `{idx_var : valeur}` qui peut être vide (par exemple pour les n_reines) ou partiellement rempli (par exemple pour les sudokus).
- un booléen pour l'activation d'AC1
- un booléen pour l'activation d'AC3
- un booléen pour l'activaton du Forward-Checking
- un booléen pour l'activation de Maintaining Arc Consistency (qui ne peut pas être effectuée en même temps que FC)
- un string décrivant la `value_order_startegy`, ce string pouvant prendre les valeurs suivantes : given_order, min_value, random_value, most_supported_value.
- un string décrivant la `variable_order_startegy`, ce string pouvant prendre les valeurs suivantes : given_order, min_domain_size, random, most_constrained.
- un double `max_duration` qui limite le temps d'éxécution du programme.
