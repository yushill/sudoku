import torch, sys, onnx, os, subprocess
import torch.nn as nn
import torch.optim as optim
from onnx import numpy_helper
import numpy as np

IMGSIDE = 28
MARGIN=1
CELLSIDE=IMGSIDE+2*MARGIN

class MNISTMLP(nn.Module):
    def __init__(self, path):
        super().__init__()
        self.fc1 = nn.Linear(784, 50)
        self.fc2 = nn.Linear(50, 50)
        self.fc3 = nn.Linear(50, 10)

        onnx_model = onnx.load(path)
        # onnx.checker.check_model(onnx_model)
        # print("Inputs: " + ' '.join(x.name for x in onnx_model.graph.input))
        # print("Ouputs: " + ' '.join(x.name for x in onnx_model.graph.output))
        # print("Opérateurs :")
        # for node in model.graph.node:
        #     print(" ", node.op_type, node.name, list(node.input), "->", list(node.output))
        # print("Paramètres :")
        # for tensor in model.graph.initializer:
        #     print(" ", tensor.name, list(tensor.dims))
            
        # Récupérer les initialiseurs sous forme de dictionnaire.
        weights = {
            tensor.name: numpy_helper.to_array(tensor)
            for tensor in onnx_model.graph.initializer
        }
    
        with torch.no_grad():
            for name, parameter in self.named_parameters():
                if name not in weights:
                    raise ValueError(f"Poids absent du modèle ONNX : {name}")
    
                tensor = torch.tensor(weights[name], dtype=parameter.dtype)
    
                if tensor.shape != parameter.shape:
                    raise ValueError(
                        f"Dimensions incompatibles pour {name}: "
                        f"{tensor.shape} != {parameter.shape}"
                    )
    
                parameter.copy_(tensor)

        self.optimizer = optim.SGD(self.parameters(), lr=1e-4)
        self.loss_fn = nn.CrossEntropyLoss()

        self.eval()

    def forward(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        return self.fc3(x)

    def get_digit(self, image):
        """
        image : input digit image.
        Try a prediction and retrain if needed.
        """

        for idx, value in enumerate(image):
            if   value < 85:  pix = ' '
            elif value < 170: pix = '+'
            else:             pix = '#'
            sys.stdout.write(pix)
            if (idx % IMGSIDE) == (IMGSIDE - 1):
                sys.stdout.write('\n')
        
        x = torch.as_tensor(image, dtype=torch.float32).reshape(1, 784)
    
        with torch.no_grad():
            logits = self(x)
            # return logits.numpy().reshape(10)
            # probabilities = torch.softmax(logits, dim=1)
            digit = int(torch.argmax(logits, dim=1).item())

        sys.stdout.write( f'[{digit}]:' )
        sys.stdout.flush()
        answer = sys.stdin.readline()

        try:
            answer = int(answer.strip())
            if answer < 0 or answer >= 10 or answer == digit:
                raise ValueError(answer)
        except ValueError:
            # Prediction succeeded
            return digit

        # Prediction failed, retrain
        self.train()
        
        y = torch.tensor([answer], dtype=torch.long)
        
        self.optimizer.zero_grad()
        logits = self(x)
        loss = self.loss_fn(logits, y)
        loss.backward()
        self.optimizer.step()
        
        sys.stdout.write( f'Retrained for {answer}, loss={loss.item()}\n' )
        
        self.eval()

        return answer

    def save(self, path):
        # Exporter le modèle actualisé.
        torch.onnx.export(
            self.eval(),
            (torch.zeros(1, 784),),
            path,
            input_names=["input"],
            output_names=["output"],
            dynamo=False,
        )

def tokens(filename):
    for line in open(filename):
        for token in line.strip().split():
            yield token
            
        
def read_pgm(filename, gridside):
    src = iter(tokens(filename))
    for hidx in range(4):
        token = next(src)
    
    if   hidx == 0 and token != "P2": raise Exception('Expecting a P5 PGM file')
    elif hidx in (1,2) and int(token) != gridside: raise Exception('Expecting valid image dimensions')
    elif hidx == 2: self.width = int(token)
    elif hidx == 3 and token != "255": raise Exception('Expecting a grayscale (256 levels) PGM file')

    return src

def main(filename):
    basename, ext = os.path.splitext(filename)
    assert ext == '.png'
    pgm = basename + '.pgm'
    subprocess.check_call(['magick', filename, '-resize', '270x270^', '-gravity', 'center', '-extent', '270x270', '-colorspace', 'Gray', '-depth', '8', '-compress', 'none', pgm])

    src = read_pgm(pgm, 9*CELLSIDE)

    loframe, hiframe = MARGIN, IMGSIDE + MARGIN

    figures = [np.zeros((IMGSIDE*IMGSIDE), dtype=np.float32) for col in range(9)]
    grid = np.zeros((9,9),dtype=np.int32)
    getter = MNISTMLP("getfigure.onnx")
    
    for row in range(9):
        for y in range(CELLSIDE):
            for col in range(9):
                for x in range(CELLSIDE):
                    val = int(next(src))
                    if y < loframe or y >= hiframe or x < loframe or x >= hiframe:
                        continue;
                    figures[col][IMGSIDE*(y-MARGIN)+(x-MARGIN)] = 255-val

        for col in range(9):
            digit = getter.get_digit(figures[col])
            grid[row,col] = digit

    txt = basename + '.txt'
    sink = open(txt,'w')
    for row in range(9):
        for col in range(9):
            val = grid[row,col]
            if val == 0: val = '.'
            sink.write(f'{val}')
        sink.write('\n')

    getter.save("getfigure.onnx")
    
if __name__ == "__main__":
    main(sys.argv[1])
    
# digit = np.array([0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 61, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 79, 74, 50, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 163, 213, 216, 215, 215, 215, 215, 215, 215, 215, 215, 218, 224, 212, 139, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 110, 148, 152, 151, 151, 151, 151, 151, 151, 151, 151, 177, 215, 203, 110, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 14, 14, 14, 14, 14, 14, 14, 14, 13, 26, 124, 229, 190, 58, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 173, 217, 151, 14, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 128, 219, 178, 62, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 76, 181, 221, 139, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 134, 219, 180, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 52, 177, 221, 123, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 163, 221, 180, 74, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 52, 191, 225, 125, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 135, 210, 185, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 58, 196, 223, 115, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 128, 214, 205, 68, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 51, 191, 213, 119, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 122, 217, 194, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 201, 213, 135, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 102, 221, 193, 50, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 79, 195, 209, 135, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 125, 227, 190, 63, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 69, 172, 205, 130, 13, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 34, 72, 72, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]).astype(np.float32)
    


