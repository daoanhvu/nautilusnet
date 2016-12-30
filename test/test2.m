function test2()
    clear; close all; clc    
    warning('off', 'Octave:possible-matlab-short-circuit-operator');
    
    % size of input image is 20x20 => we have 400 nodes in input layer
    input_layer_size = 4;
    hidden_layer_size = 5;
    num_labels = 2;
    
    X = [0 1.3 1.5 1];
    y = [1];
    
    %5x5 mat
    Theta1 = [  0.01 0.015 0.3 0.5 0.1;...
                0.18 0.06 0.1 0.25 0.71;... 
                0.14 0.05 0.23 0.39 0.2;...
                0.21 0.18 0.54 0.4 0.21;...
                0.17 0.11 0.87 0.2 0.09];
    %2x6 mat
    Theta2 = [  0.23 0.18 0.4 0.74 0.12 0.35;...
                0.66 0.27 0.54 0.4 0.21 0.5;];
    
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
    a1
    Theta1'
    z2 = a1 * Theta1';
    z2
    a2 = sigmoid(z2);
    a2 = [ones(size(a2, 1), 1) a2];
    a2
    Theta2
    Theta2'
    z3 = a2 * Theta2';
    z3
    a3 = sigmoid(z3);
    a3
    
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