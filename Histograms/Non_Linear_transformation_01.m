% Чтение низкоконтрастного цветного изображения
I = imread('Histograms/photo.jpeg');  

% Получение размеров изображения
[numRows, numCols, Layers] = size(I);

% Извлечение отдельных каналов RGB
I_red = I(:, :, 1);  
I_green = I(:, :, 2); 
I_blue = I(:, :, 3);  

% Применение адаптивного выравнивания гистограммы к каждому каналу
I_red_eq = histogram_equalization(I_red, numRows, numCols);
I_green_eq = histogram_equalization(I_green, numRows, numCols);
I_blue_eq = histogram_equalization(I_blue, numRows, numCols);

% Применение Рэлеевского преобразования для каждого канала с параметром sigma
sigma = 50;  % Параметр σ для Рэлеевского распределения (можно изменять в зависимости от изображения)
I_red_stretch = rayleigh_transform(I_red_eq, sigma);
I_green_stretch = rayleigh_transform(I_green_eq, sigma);
I_blue_stretch = rayleigh_transform(I_blue_eq, sigma);

% Объединение обработанных каналов в цветное изображение
I_enhanced = cat(3, I_red_stretch, I_green_stretch, I_blue_stretch);

% Визуализация исходного изображения, результатов обработки и гистограмм
figure;

% Исходное изображение
subplot(3, 4, 1), imshow(I), title('Исходное цветное изображение');
% Красный канал и его гистограмма
subplot(3, 4, 2), imshow(I_red), title('Красный канал');
subplot(3, 4, 3), imshow(I_red_stretch), title('Красный канал после Рэлеевского преобразования');
subplot(3, 4, 4), imhist(I_red), title('Гистограмма красного канала');
% Зеленый канал и его гистограмма
subplot(3, 4, 5), imshow(I_green), title('Зеленый канал');
subplot(3, 4, 6), imshow(I_green_stretch), title('Зеленый канал после Рэлеевского преобразования');
subplot(3, 4, 7), imhist(I_green), title('Гистограмма зеленого канала');
% Синий канал и его гистограмма
subplot(3, 4, 8), imshow(I_blue), title('Синий канал');
subplot(3, 4, 9), imshow(I_blue_stretch), title('Синий канал после Рэлеевского преобразования');
subplot(3, 4, 10), imhist(I_blue), title('Гистограмма синего канала');
% Улучшенное изображение
subplot(3, 4, 11), imshow(I_enhanced), title('Улучшенное цветное изображение');

% Ручная реализация функции выравнивания гистограммы
% Метод: Выравнивание гистограммы (Histogram Equalization)
function I_eq = histogram_equalization(I_channel, numRows, numCols)
    % Вычисление гистограммы
    hist_counts = zeros(256, 1);
    for i = 1:numRows
        for j = 1:numCols
            pixel_value = I_channel(i, j);
            hist_counts(pixel_value + 1) = hist_counts(pixel_value + 1) + 1;
        end
    end

    % Вычисление кумулятивной функции распределения (CDF)
    cdf = cumsum(hist_counts) / (numRows * numCols);
    cdf_min = min(cdf(cdf > 0));  % Нахождение минимального ненулевого значения CDF

    % Использование CDF для корректировки значений пикселей
    I_eq = zeros(numRows, numCols);
    for i = 1:numRows
        for j = 1:numCols
            pixel_value = I_channel(i, j);
            I_eq(i, j) = round((cdf(pixel_value + 1) - cdf_min) / (1 - cdf_min) * 255);
        end
    end
    I_eq = uint8(I_eq);  % Преобразование обратно в тип uint8
end

% Ручная реализация Рэлеевского преобразования (распределение Рэлея)
function I_out = rayleigh_transform(I_channel, sigma)
    % Преобразование по закону Рэлея
    I_channel = double(I_channel);  % Преобразование в тип double для расчётов
    I_out = (I_channel / sigma^2) .* exp(-I_channel.^2 / (2 * sigma^2));
    
    % Нормализация к диапазону [0, 255]
    I_out = I_out - min(I_out(:));  % Вычитаем минимальное значение
    I_out = I_out / max(I_out(:)) * 255;  % Масштабируем к диапазону [0, 255]
    I_out = uint8(I_out);  % Преобразуем обратно в тип uint8
end
