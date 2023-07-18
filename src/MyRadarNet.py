from torch import nn

class MyRadarNet(nn.Module):   
    def __init__(self, frames_count):
        super(MyRadarNet, self).__init__()

        self.cnn_layers = nn.Sequential(
            nn.Conv2d(frames_count, 32, kernel_size=3),
            nn.BatchNorm2d(32),
            nn.ReLU(),           
            
            nn.Conv2d(32, 32, kernel_size=3),
            nn.BatchNorm2d(32),
            nn.ReLU(),
            nn.MaxPool2d(kernel_size=2, stride=2),
            
            nn.Conv2d(32, 64, kernel_size=3),
            nn.BatchNorm2d(64),
            nn.ReLU(),
        )

        self.linear_layers = nn.Sequential(
            nn.Flatten(),
            nn.Linear(1664, 3), 
            nn.Softmax(dim=1)                   
        )

    def forward(self, x):
        x = self.cnn_layers(x)
        x = self.linear_layers(x)
        return x
