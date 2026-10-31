import argparse
import onnxruntime
import numpy as np
import torchvision.transforms as transforms
from torchvision.datasets import CIFAR10

class Inference():
    def __init__(self, onnx):
        '''Loader ONNX Model'''
        self.ort_session = onnxruntime.InferenceSession(onnx)

    def dataset(self, path):
        '''Prepare CIFAR-10 image data'''
        '''Normalize : (0.5, 0.5, 0.5), (0.5, 0.5, 0.5)'''
        transform = transforms.Compose([
            transforms.Resize((32, 32)),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.485, 0.456, 0.406],std=[0.229, 0.224, 0.225])
        ])

        '''Loader CIFAR-10 image data'''
        self.testset = CIFAR10(root=path, train=False, download=True, transform=transform)
    
    def run(self, images):
        '''Inference and calculate accuracy'''
        correct = 0
        total = 0
        for i in range(images):  # Inferenc 100 images
            images, labels = self.testset[i]
            
            # add batch dimension
            images = images.unsqueeze(0)
            
            # inference
            ort_inputs = {self.ort_session.get_inputs()[0].name: images.numpy()}
            ort_outputs = self.ort_session.run(None, ort_inputs)
            
            # Parse the output and calculate the accuracy
            probabilities = ort_outputs[0][0]
            predicted_class = np.argmax(probabilities)
            total += 1
            if predicted_class == labels:
                correct += 1

        accuracy = correct / total
        print("Accuracy:", accuracy)


def get_parser():
    '''
    parameter analysis
    '''
    parser = argparse.ArgumentParser(description='Inference verification')
    parser.add_argument('--loader_onnx', metavar='location', default='./save/resnet20/resnet20.onnx', help='loader onnx file')
    parser.add_argument('--datasets', metavar='datasets', default='/opt/datasets', help='Datasets')
    parser.add_argument('--images', metavar='images', type=int, default=100, help='Inference quantity')

    return parser

def main():
    '''main function entry'''
    parser      = get_parser()
    args        = parser.parse_args()

    model = Inference(args.loader_onnx)
    model.dataset(args.datasets)
    model.run(args.images)

if __name__ == '__main__':
    main()

