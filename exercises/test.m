function test()
    clear; close all; clc    
    warning('off', 'Octave:possible-matlab-short-circuit-operator');
    
    % size of input image is 20x20 => we have 400 nodes in input layer
    input_layer_size = 400;
    hidden_layer_size = 25;
    num_labels = 10;
    
    load('ex4data1.mat');
    %m = size(X, 1);
    
    X = X(1, :);
    
    size(X)
    
    load('ex4weights.mat');
    
    %Unroll parameter
    nnParams = [Theta1(:); Theta2(:)];
    lambda = 0;
    
    %J = nnCostFunction(nnParams, input_layer_size, hidden_layer_size, num_labels, X, y, lambda);
    %printf("\nJ = %f\n", J);
    
                                        
    %Number of training example
    m = size(X, 1);
    
    % Initial output values
    %J = 0;
    grad1 = zeros(size(Theta1));
    grad2 = zeros(size(Theta2));
    
    % Part 1: Feedforward and return the cost in J variable
    %add column 1s to X and assign to a1 for bias
    a1 = [ones(m, 1) X];
    z2 = a1 * Theta1';
    size(z2)
    a2 = sigmoid(z2);
    a2 = [ones(size(a2, 1), 1) a2];
    
    z3 = a2 * Theta2';
    a3 = sigmoid(z3);
    
    hThetaX = a3;
    yVec = zeros(m, num_labels);
    
    for i=1:m
        yVec(i, y(i)) = 1;
    end
    
    J = (1.0/m) * sum(sum(-yVec .* log(hThetaX) - (1-yVec) .* log(1-hThetaX)));
    
    printf("\nJ = %f (Before regularization) \n", J);
    
    regularator = (lambda/(2*m)) * (sum(sum(Theta1(:, 2:end).^2)) + sum(sum(Theta2(:, 2:end).^2)) );
    
    J = J + regularator;
    
    printf("\nJ = %f\n", J);
    
    %Part 2:
    for i=1:m
        a1 = [1; X(i, :)'];
        
        z2 = Theta1 * a1;
        a2 = [1; sigmoid(z2)];
        
        z3 = Theta2 * a2;
        a3 = sigmoid(z3);
        
        %Actual output
        yy = ([1:num_labels] == y(i))';
        d3 = a3 - yy;
        
        d2 = (Theta2' * d3) .* [1; sigmoidGradient(z2)];
        d2 = d2(2:end);
        
        grad1 = grad1 + d2 * a1';
        grad2 = grad2 + d3 * a2';
    end
    
    grad1 = (1/m) * grad1 + (lambda/m) * [zeros(size(Theta1, 1), 1) Theta1(:,2:end)];
    grad2 = (1/m) * grad2 + (lambda/m) * [zeros(size(Theta2, 1), 1) Theta2(:,2:end)];
    
    grad = [grad1(:);grad2(:)];
end