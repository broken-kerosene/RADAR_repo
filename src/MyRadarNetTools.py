import torch
import numpy as np
import matplotlib.pyplot as plt
from IPython.display import clear_output
from sklearn.metrics import accuracy_score

def make_dataloader(X_train, X_test, y_train, y_test, batch = 128):
    X_train = torch.Tensor(X_train)
    y_train = torch.Tensor(y_train).cuda().long()

    X_test = torch.Tensor(X_test)
    y_test = torch.Tensor(y_test).cuda().long()

    print(X_train.shape, y_train.shape, X_test.shape, y_test.shape)
    train_dataset = torch.utils.data.TensorDataset(X_train, y_train)
    test_dataset = torch.utils.data.TensorDataset(X_test, y_test)

    batch = 128
    train_dataloader = torch.utils.data.DataLoader(train_dataset, shuffle=True, batch_size=batch)
    val_dataloader = torch.utils.data.DataLoader(test_dataset, shuffle=True, batch_size=batch)
    return train_dataloader, val_dataloader


def train_one_epoch(model, train_dataloader, criterion, optimizer, epoch_index, device="cuda:0"):
    model.train()   
    num = 0
    res_loss = []
    for x_, y_ in (train_dataloader):
        x_train, y_train = x_.to(device), y_.to(device) # перенос вычислений на GPU
        y_pred = model(x_train)        
        loss = criterion(y_pred, y_train) 
        loss.backward()
        optimizer.step()
        optimizer.zero_grad()        
        res_loss.append(loss.item())
        num += 1
    return np.mean(res_loss)

def predict(model, val_dataloader, criterion, device="cuda:0"):
    model.to(device)
    model.eval()
    losses = []
    true_classes = []
    predicted_classes = []
    total = 0
    correct = 0
    with torch.no_grad():
        for x_, y_ in (val_dataloader):
            x_test, y_test = x_.to(device), y_.to(device) # перенос вычислений на GPU           
            y_pred = model(x_test)
            loss = criterion(y_pred, y_test)
            losses.append(loss.item())
            
            _, predicted = torch.max(y_pred.data, 1)
            predicted_classes.extend(predicted.cpu().numpy().tolist())
            true_classes.extend(y_test.cpu().numpy().tolist())
            
            total += y_test.size(0)
            correct += (predicted == y_test).sum().item()
       
    return np.mean(losses), predicted_classes, true_classes


def train(model, train_dataloader, val_dataloader, criterion, optimizer, scheduler=None, device="cuda:0", n_epochs=10):
    model.to(device)
    best_acc = 0
    metrics = {'epoch': [], 'train_loss': [], 'test_lose': [], 'accuracy': [] }
    ep = []
    for epoch in (range(1, n_epochs+1)):
        train_loss = train_one_epoch(model, train_dataloader, criterion, optimizer, epoch, device)       
        valid_loss, predicted, true = predict(model, val_dataloader, criterion, device)
        accuracy = accuracy_score(predicted, true)
        metrics['epoch'].append(epoch)
        metrics['train_loss'].append(train_loss)
        metrics['test_lose'].append(valid_loss)
        metrics['accuracy'].append(accuracy * 100)
        
        clear_output(True)
        plot_losses(metrics)
             
        if best_acc < accuracy:
            best_acc = accuracy
            save_model(model, best_acc)
        if scheduler != None:
            scheduler.step()
            
def plot_losses(metrics):
        print(f"Эпоха: {metrics['epoch'][-1]};\t"
            + f"train loss: {metrics['train_loss'][-1]:.3f};\t"
            + f"valid loss: {metrics['test_lose'][-1]:.3f};\t"
            + f"Точность: {metrics['accuracy'][-1]:.2f} % ")
        x = metrics['epoch']
        fig = plt.figure(figsize=(12,4))
        plt.subplot(1, 2, 1)
        plt.plot(x, metrics['train_loss'], x, metrics['test_lose'])
        plt.title('Loss')
        plt.legend(metrics.keys(), loc=2)
        plt.ylim([0.4, 1])
        plt.grid()

        plt.subplot(1, 2, 2)
        plt.plot(x, metrics['accuracy'])
        plt.title('accuracy')
        plt.legend(['accuracy'], loc=2)
        plt.ylim([70, 100])
        plt.grid()
        plt.show()

def save_model(model, best_acc, path_dir="model/",):
    torch.save(model, f"{path_dir}MyRadarNet_acc_{100 * best_acc:.2f}.pth")
    torch.save(model.state_dict(), f"{path_dir}MyRadarNet_acc_{100 * best_acc:.2f}_w.pt")