//Desenvolvedor: Leonardo de Souza Vieira
//Data: 20/10/2016

#include "../include/mlp.h"
#include "../include/openMatrixFile.h"
#include "../include/activationFunctions.h"

Neuron *initNeuronRandom(int links, double a){
  Neuron *new;
  int i;

  new = malloc(sizeof(Neuron));
  new->num_weigths = links;
  new->a = a;
  new->error = 0.0;
  if(links != 0){
    new->weights =  malloc(sizeof(double) * links);
  }
  else
  {
        new->weights = NULL;
  }


  for(i = 0; i < links; i++){
    new->weights[i] = (double)(rand() % 200 - 100) / 100;
  }

  return new;
}

Layer *initLayerRandom(int s, int links_neurons){
  Layer *new;
  int i;

  new = malloc(sizeof(Layer));
  new->size = s;
  new->links =links_neurons;
  new->bias = initNeuronRandom(links_neurons, 1.0);
  new->neurons =  malloc(sizeof(Neuron)*s);

  for(i = 0; i < s;i++){
    new->neurons[i] = initNeuronRandom(links_neurons,0);
  }
  return new;

}

MLP *initMLP(int in, int out, int hidden, int n_hiddens){

	MLP		*mlp;
	int		i;

	mlp = malloc(sizeof(MLP));
  mlp->size_in = in;
  mlp->size_out = out;
  mlp->size_hidden = hidden;
  mlp->num_hiddens = n_hiddens;

  mlp->input_layer = initLayerRandom(in, hidden);
  mlp->output_layer = initLayerRandom(out,0);

  mlp->hidden_layers = malloc(sizeof(Layer)*n_hiddens);
  for(i = 0; i < n_hiddens;i++)
    mlp->hidden_layers[i] = initLayerRandom(hidden,(i == n_hiddens-1?out:hidden));

  return mlp;

}

void freeNeuron(Neuron *n){
  free(n->weights);
  free(n);

}

void freeLayer(Layer *l){
  int i;
  freeNeuron(l->bias);
  for (i = 0; i < l->size; i++)
		freeNeuron(l->neurons[i]);
	free(l->neurons);
}

void freeMLP(MLP *mlp){
  int i;

  freeLayer(mlp->input_layer);
  freeLayer(mlp->output_layer);

  for(i = 0; i < mlp->num_hiddens;i++)
    freeLayer(mlp->hidden_layers[i]);

  free(mlp);
}

void setErrorNeuron(double value, Neuron *n){
  n->error = value;
}
void setActivationNeuron(Neuron *n, double value){
  // printf("%f\n",n->a);
  // printf("%f\n",value);
  n->a = value;
  // printf("%f\n",n->a);
}

void setWeigthsNeuron(Neuron *n, double *values){
  int i;
  for(i = 0;i < n->num_weigths;i++)
    n->weights[i] = values[i];

}

void printNeuron(Neuron *n){
  printf("%.8f\n\n",n->a);
}


void printNeuronWeights(Neuron *n){
  int i;

  for(i = 0;i < n->num_weigths;i++)
    printf("%.8f ",n->weights[i]);
  printf("\n\n");

}

void setLayer(Layer *l, double *values){
  int i;
  for(i = 0; i < l->size;i++){
     setActivationNeuron(l->neurons[i], values[i]);
  }

}

void printLayer(Layer *l){
  int i;
  printf("bias -> ");
  printNeuron(l->bias);
  for(i = 0;i < l->size;i++){
    printf("%d -> ",i);
    printNeuron(l->neurons[i]);
  }

}


void printLayerWeights(Layer *l){
  int i;
  printf("\nBias-> ");
  printNeuronWeights(l->bias);
  for(i = 0;i < l->size;i++){
    printf("%d -> ",i );
    printNeuronWeights(l->neurons[i]);
  }

  printf("\n");
}

void printValuesMLP(MLP *mlp){
  int i;
  printf("\n--------MLP VALUES---------\n");
  printf("\n--------InputLayer---------\n");
  printLayer(mlp->input_layer);

  printf("\n------HiddenLayers---------\n");
  for(i = 0; i < mlp->num_hiddens; i++)
    printLayer(mlp->hidden_layers[i]);

  printf("\n------OutputLayer---------\n");
    printLayer(mlp->output_layer);
  printf("\n---------End---------------\n");
}

void printWeightsMLP(MLP *mlp){
  int i;
  printf("\n--------MLP WEIGHTS---------\n");
  printf("\n--------InputLayer---------\n");
  printLayerWeights(mlp->input_layer);

  printf("\n------HiddenLayers---------\n");
  for(i = 0; i < mlp->num_hiddens; i++)
    printLayerWeights(mlp->hidden_layers[i]);

  printf("\n------OutputLayer---------\n");
    printLayerWeights(mlp->output_layer);
  printf("\n---------End---------------\n");
}

void layerForwardPropagation(Layer *back_layer, Layer *current_layer){
  int i,j, k;
  double result = 0.0, b = 0.0;

  //camada forwardPropagation
  for(i = 0; i < current_layer->size;i++){
    b = (back_layer->bias->a) * (back_layer->bias->weights[i]);

    for(j = 0; j < back_layer->size;j++)
      result = result + (((back_layer->neurons[j])->a) * ((back_layer->neurons[j])->weights[i]));

    result = sigmoid(result + b);

    setActivationNeuron(current_layer->neurons[i], result);
    result = 0.0;

  }

}

// void linearFunctionOut(Layer *back_layer, Layer *current_layer){
//   int i,j, k;
//   double result = 0.0, b = 0.0;
//
//   //camada forwardPropagation
//   for(i = 0; i < current_layer->size;i++){
//     b = (back_layer->bias->a) * (back_layer->bias->weights[i]);
//
//     for(j = 0; j < back_layer->size;j++)
//       result = result + (((back_layer->neurons[j])->a) * ((back_layer->neurons[j])->weights[i]));
//
//     result = result + b;
//
//     setActivationNeuron(current_layer->neurons[i], result);
//     result = 0.0;
//
//   }
//
// }

void forwardPropagation(MLP *mlp){
  int i, k;

  //Primeira camada escondida
  layerForwardPropagation(mlp->input_layer,mlp->hidden_layers[0]);

  //as camadas escondidas restantes começando da segunda camada k = 1
  for(i = 1; i < mlp->num_hiddens; i++)
    layerForwardPropagation(mlp->hidden_layers[i - 1], mlp->hidden_layers[i]);
  k = (mlp->num_hiddens) - 1;
  layerForwardPropagation(mlp->hidden_layers[k], mlp->output_layer);

}

// void forwardPropagation(MLP *mlp){
//   int i,j, k;
//   double result = 0.0, b = 0.0;
//
//   //primeira camada escondida
//   for(i = 0; i < mlp->size_hidden;i++){
//     b = (mlp->input_layer->bias->a) * (mlp->input_layer->bias->weights[i]);
//
//     for(j = 0; j < mlp->size_in;j++)
//       result = result + (((mlp->input_layer->neurons[j])->a) * ((mlp->input_layer->neurons[j])->weights[i]));
//
//     result = result + b;
//
//     setActivationNeuron(mlp->hidden_layers[0]->neurons[i], result);
//     result = 0.0;
//
//   }
//   //as camadas escondidas restantes começando da segunda camada k = 1
//   for(k = 1; k< mlp->num_hiddens;k++){
//     for(i = 0; i < mlp->size_hidden;i++){
//       b = (mlp->hidden_layers[k - 1]->bias->a) * (mlp->hidden_layers[k -1]->bias->weights[i]);
//
//       for(j = 0; j < mlp->size_hidden;j++)
//         result = result + (((mlp->hidden_layers[k -1]->neurons[j])->a) * ((mlp->hidden_layers[k -1]->neurons[j])->weights[i]));
//
//       result = result + b;
//
//       setActivationNeuron(mlp->hidden_layers[k]->neurons[i], result);
//       result = 0.0;
//
//     }
//   }
//   k = mlp->num_hiddens -1;
//   //camada de saída
//   for(i = 0; i < mlp->size_out;i++){
//     b = (mlp->hidden_layers[k]->bias->a) * (mlp->hidden_layers[k]->bias->weights[i]);
//
//     for(j = 0; j < mlp->size_hidden;j++)
//      result = result + (((mlp->hidden_layers[k]->neurons[j])->a) * ((mlp->hidden_layers[k]->neurons[j])->weights[i]));
//
//     result = result + b;
//
//     setActivationNeuron(mlp->output_layer->neurons[i], result);
//     result = 0.0;
//   }
//
// }



void backPropagation(MLP *mlp, double **mat, double *grad){
  int i;

  for(i = 0; i < 3; i++){
    setLayer(mlp->input_layer,mat[i]);
    forwardPropagation(mlp);
    //printValuesMLP(mlp);

  }


}

main() {
  int i;

  double **mat = createMatrix(NUM_ROWS, NUM_COLUMNS);
  double *grad = createVector(SIZE_GRADIENT);
  MLP *mlp = initMLP(SIZE_INPUT, SIZE_OUTPUT, SIZE_HIDDEN, NUM_HIDDENS);
  readFileMatrix(mat, NUM_ROWS, NUM_COLUMNS);
//  backPropagation(mlp,mat,grad);


  setLayer(mlp->input_layer,mat[0]);

  forwardPropagation(mlp);
  //printLayerWeights(mlp->input_layer);
  //printWeightsMLP(mlp);
  printValuesMLP(mlp);
  //printMatrix(mat, NUM_ROWS, NUM_COLUMNS);

  freeMatrix(mat, NUM_ROWS);
  freeMLP(mlp);


  return 0;
}
