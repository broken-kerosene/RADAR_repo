import torch
import torch.optim as optim
from sklearn.model_selection import train_test_split

from src.tools import *
from src.MyRadarNetTools import *
from src.MyRadarNet import *

# import random
# SEED = 42
# random.seed(SEED)
# np.random.seed(SEED)
# torch.manual_seed(SEED)
# torch.cuda.manual_seed(SEED)
# torch.backends.cudnn.deterministic = True

def main() -> int:
    device = torch.device("cuda:0") if torch.cuda.is_available() else torch.device("cpu")
    print("Device is ready: ", device)
    frames_count = 3
    x, y, _ = read_Files(frames_count)   
    print(x.shape, y.shape)
    X_train, X_test, y_train, y_test = train_test_split(x, y, test_size=0.2)
    train_dataloader, val_dataloader = make_dataloader(X_train, X_test, y_train, y_test, batch=64)

    model = MyRadarNet(frames_count)
    if device == "cuda:0":
        model.cuda()

    n_epochs = 50
    optimizer = torch.optim.Adam(model.parameters(), lr=0.00005, eps=1e-08)
    criterion = nn.CrossEntropyLoss()
    scheduler = optim.lr_scheduler.ExponentialLR(optimizer, gamma=0.97)

    torch.cuda.empty_cache()
    train(model, train_dataloader, val_dataloader, criterion, optimizer, scheduler, device, n_epochs)

    _, predicted_labels, true_labels = predict(model, val_dataloader, criterion, device)
    plot_conf_matrix(predicted_labels, true_labels)

    return 0

if __name__ == '__main__':
    main()
