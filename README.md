---
layout: post
title: Viola-Jones Yöntemi ile Nesne Tespiti (Object Detection)
date: '2019-02-22T22:48:00.000+03:00'
author: Bahri ABACI
categories:
- Görüntü İşleme Uygulamaları
- Nesne Tespiti
modified_time: '2019-07-14T23:06:47.017+03:00'
thumbnail: /assets/post_resources/object_detection/thumbnail.png
---

Nesne tespiti (object detection) karmaşık bir imge içerisinde spesifik/hedeflenen bir nesnenin var olup olmadığını, var ise hangi piksel bölgesi içerisinde yer aldığını belirleme işlemdir. Klasik yöntemlere göre çalışan bilgisayarlı görü uygulamalarında nesne tespiti; imge ön işleme (gri seviye dönüşüm, histogram eşitleme, eşikleme vb.) adımından sonra, nesne tanıma/sınıflandırma (bitki tanıma, yüz tanıma, plaka tanıma) adımından önce yapılan bir işlemdir. Nesne tespiti ile belirli bir nesneyi sınıflandırmak için eğitilmiş bir sistemin girdileri filtrelenerek, sınıflandırma sisteminin girdileri kontrol altına alınır. Bu sayede yüz tanıma için eğitilmiş akıllı bir sistemin yüz olmayan bölgelerde çalışması engellenir ve olası yanlış sınıflandırmaların önüne geçilir.

<!--more-->

Nesne tespiti bilgisayarlı görü camiasının uzun süredir üzerinde çalıştığı ve çok başarılı çıktılar ürettiği bir alandır. Bu yazımızda 2001 yılında Paul Viola ve Michael Jones tarafından önerilen ve özellikle yüz tespiti alanında hala sıklıkla tercih edilen Viola-Jones algoritmasını inceleyeceğiz. Viola-Jones algoritması her ne kadar nesne tespiti algoritması olsa da, konuyu yüz tespiti üzerinden anlatmak ve anlamanın daha kolay olacağını düşündüğümden anlatımlarımı bu alt başlık üzerinden yapacağım.

Viola-Jones algoritması bir pencere içerisinde (tipik pencere boyutu 24x24) yer alan piksel bloklarının bir biri ile olan ilşkisini öğrenme prensibi üzerine kurulmuştur. Bu ilişkiyi öğrenebilmek için aşağıda örnekleri gösterilen ve literatürde haar dalgacıkları olarak bilinen öznitelikler ve AdaBoost öğrenme algoritması kullanılmıştır.

![haar dalgacıkları][haar_wavelets]

### Öznitelik Çıkarımı
Görselleri verilen haar dalgacıklarda siyah pikseller negatif değerleri, beyaz pikseller ise pozitif değerleri göstermek için kullanılmıştır. Evrişim mantığına oldukça benzer şekilde; öznitelikler imge üzerinde gezdirilmekte ve beyaz bölge altında kalan piksellerin toplamı beyaz bölgenin katsayısı ile çarpılmakta, siyah bölge altında kalan piksellerin toplamı ile siyah bölgenin katsayısı çarpılmakta ve ardından bu sayılar toplanmaktadır. !["N"](https://render.githubusercontent.com/render/math?math=N) görüntüdeki piksel sayısı, !["k"](https://render.githubusercontent.com/render/math?math=k) öznitelikteki toplanacak piksel sayısı olmak üzere bu işlemin karmaşıklığı !["O(Nk)"](https://render.githubusercontent.com/render/math?math=O%28Nk%29) dır.

Bu işlemin her pikselde yüzlerce öznitelik için yapılması düşünüldüğünde işlemin oldukça uzun sürmesi beklenmektedir. Ancak Viola-Jones tarafından önerilen [tümlev imge](http://www.cescript.com/2019/02/tumlev-imge-integral-image.html) (integral image) yaklaşımı özniteliklerin hesaplanması karmaşıklığını !["k"](https://render.githubusercontent.com/render/math?math=k) öznitelik boyutundan arındırarak, !["O(N)"](https://render.githubusercontent.com/render/math?math=O%28N%29) seviyesine indirmektedir.

### Adaboost ile Öğrenme
Adaboost yöntemi ile öğrenme aşamasında ise şu adımlar izlenmektedir: İlk olarak ağırlık vektörü !["w"](https://render.githubusercontent.com/render/math?math=w) her bir  imgeye eşit ağırlık verilerek ve seçilen sınıflandırıcılar kümesi !["h"](https://render.githubusercontent.com/render/math?math=h)  boş küme ile ilklenir. Ardından her adımda, özniteliklerin her biri yüz  ve yüz olmayan binlerce imgeye (eğitim setine) uygulanır ve imgelerinin  bu filtrelere verdiği yanıtlar kaydedilir. Her bir öznitelik için bu  yanıtlar kullanılarak eğitim setini en küçük hata ile ayıran bir eşik  değeri bulunur ve en küçük hatayı üreten öznitelik seçilir. Seçilen bu  öznitelik h kümesine eklenir. Son aşamada belirlenen sınıflandırıcının  çıktıları kullanılarak doğru sınıflandırılan örneklerin ağırlığı  azaltılır, yanlış sınıflandırılan örneklerin ağırlığı artırılır.  Ardından yeni ağırlıklar ile yeni bir öznitelik belirleme işlemine  geçilir. Bu işlem belirli sayıda (!["T"](https://render.githubusercontent.com/render/math?math=T) tane) sınıflandırıcı belirlenene  kadar devam edilir.

Algoritmanın çalışma prensibine ilişkin sözde kod aşağıdaki tabloda verilmiştir. 

* Eğitim setinde yer alan her olumlu örnek !["x_i"](https://render.githubusercontent.com/render/math?math=x_i) için !["y_i=1"](https://render.githubusercontent.com/render/math?math=y_i%3d1), her olumsuz örnek !["x_i"](https://render.githubusercontent.com/render/math?math=x_i) için !["y_i=0"](https://render.githubusercontent.com/render/math?math=y_i%3d0) etiketi atanır. !["N"](https://render.githubusercontent.com/render/math?math=N) olumlu ve !["M"](https://render.githubusercontent.com/render/math?math=M) olumsuz örnek sayısı olmak üzere, ağırlık vektörü !["w_{1,i} = {\frac{1}{2N},\frac{1}{2M}}"](https://render.githubusercontent.com/render/math?math=w_%7b1%2ci%7d%20%3d%20%7b%5cfrac%7b1%7d%7b2N%7d%2c%5cfrac%7b1%7d%7b2M%7d%7d) her bir imgeye eşit ağırlık verilerek ilklenir.

* Her iterasyon !["t = 1:T"](https://render.githubusercontent.com/render/math?math=t%20%3d%201%3aT) Ağırlıklar olasılık dağılımı olacak şekilde normalize edilir ![" w_{t,i} = \frac{w_{t,i}}{\sum_{j=1}^{M+N} w_{t,j}}"](https://render.githubusercontent.com/render/math?math=%20w_%7bt%2ci%7d%20%3d%20%5cfrac%7bw_%7bt%2ci%7d%7d%7b%5csum_%7bj%3d1%7d%5e%7bM%2bN%7d%20w_%7bt%2cj%7d%7d)

* Her !["h_j"](https://render.githubusercontent.com/render/math?math=h_j) özniteliğini eğitim setine uygula ve kalan hatayı !["\epsilon_j = \sum_i w_i\lVert h_j(x_i) - y_i\rVert"](https://render.githubusercontent.com/render/math?math=%5cepsilon_j%20%3d%20%5csum_i%20w_i%5clVert%20h_j%28x_i%29%20-%20y_i%5crVert) hesapla. 

* En küçük hatayı !["\epsilon_t"](https://render.githubusercontent.com/render/math?math=%5cepsilon_t) veren !["h_t"](https://render.githubusercontent.com/render/math?math=h_t) özniteliğini belirle. Doğru sınıflandırılan her örnek için !["\beta_i=\left({\frac{\epsilon_t}{1-\epsilon_t}}\right)"](https://render.githubusercontent.com/render/math?math=%5cbeta_i%3d%5cleft%28%7b%5cfrac%7b%5cepsilon_t%7d%7b1-%5cepsilon_t%7d%7d%5cright%29) olmak üzere !["w_{t+1,i}=w_{t,i}\beta_i"](https://render.githubusercontent.com/render/math?math=w_%7bt%2b1%2ci%7d%3dw_%7bt%2ci%7d%5cbeta_i) 

* Nihai sınıflandırıcıyı !["\alpha_t = -\log{\beta_t}"](https://render.githubusercontent.com/render/math?math=%5calpha_t%20%3d%20-%5clog%7b%5cbeta_t%7d) olmak üzere
![" h(x)= \begin{cases}1, & \sum_{t=1}^{T}\alpha_t h_t(x) \geq \frac{1}{2}\sum_{t=1}^{T}\alpha_t \\0, & \text{otherwise}\end{cases} "](https://render.githubusercontent.com/render/math?math=%20h%28x%29%3d%20%5cbegin%7bcases%7d1%2c%20%26%20%5csum_%7bt%3d1%7d%5e%7bT%7d%5calpha_t%20h_t%28x%29%20%5cgeq%20%5cfrac%7b1%7d%7b2%7d%5csum_%7bt%3d1%7d%5e%7bT%7d%5calpha_t%20%5c%5c0%2c%20%26%20%5ctext%7botherwise%7d%5cend%7bcases%7d%20)
olarak belirle.

Belirli sayıda öznitelik belirlendikten sonra sınıflandırıcı çalıştırılarak eğitim kümesi sınıflandırılmaktadır. Bu sınıflandırma işleminden sonra yüz olmadığına kanaat getirilen eğitim örnekleri atılarak kalan eğitim kümesi ile öğrenme işlemine devam edilir. Bu kaskat yapı sayesinde hem yüz olmadığı kolaylıkla anlaşılan örneklerin süreci yavaşlatmasının önüne geçilir hem de geriye kalan hatalı örneklerin ağırlığı artırıldığından daha yetkin bir sınıflandırıcı öğrenilmesi sağlanır.

### Test Aşaması
Test aşamasında ise imge üzerinde gezilerek öğrenilen bu öznitelik setlerinin sonuçları hesaplanır. Her kaskat sonrası elde edilen değerler öğrenilmiş değerler ile kıyaslanarak incelenen alt pencerenin yüz olup olmadığına karar verilir. Eğer herhangi bir aşamada alt pencerenin yüz içermediğine karar verilirse ileri aşamada yer alan öznitelikler incelenmeden bir sonraki alt pencereye geçilir.

Aşağıda IMLAB görüntü işleme kütüphanesi kullanılarak Viola-Jones özniteliklerinin iki farklı test imgesi üzerinde nasıl çalıştığı gösterilmiştir. İmge üzerinde kayan pencere yüz olup olmadığı test edilen pencereyi, pencerenin rengi ise (kırmızıdan yeşile) ise kaskat sınıflandırıcının kaçıncı kaskatta olduğunu göstermektedir. Pencere içerisinde beliren dikdörtgenler ise testte kullanılan haar özniteliklerini göstermektedir. Görselliği iyileştirmek amacı ile negatif katsayılı öznitelikler siyah, pozitif katsayılı öznitelikler beyaz ile renklendirilmiştir.

| Viola Jones Başarılı Tespit Adımları             |  Viola Jones Başarısız Tespit Adımları |
:-------------------------:|:-------------------------:
[![Viola-Jones Face detection Steps](/assets/post_resources/object_detection/viola_jones_face_detection.gif)](https://drive.google.com/file/d/1tuXJbGSdhaDjG7HZs-q38Q7ZO6w_fReF/preview "Link to original video") | [![Viola-Jones Face detection Steps](/assets/post_resources/object_detection/cv_dazzle_false_negative.gif)](https://drive.google.com/file/d/1hbpdcJnGXfOGc_LeFoFzScPI-xZaXt4H/preview "Link to original video")

Videodan yüz olmayan bölgelerin bir kaç kaskat incelemesinden sonra elenirken, yüz olma potansiyeli bulunan bölgelerin daha detaylı (daha fazla sayıda haar özniteliği ile) incelendiği görülmektedir. Bu kaskat yapı Viola-Jones algoritmasının performansını artıran önemli bir bileşendir.

Bu yazımızda Viola-Jones algoritmasını öğrenilmiş bir öznitelik havuzunu kullanarak gerçeklemeye çalışacağız. Kullanacağımız öznitelik seti OpenCV topluluğu tarafından yüz tespiti için eğitilen ve haarcascade_frontalface_alt ismi ile paylaşılan haar öznitelik seti olacak ve kodlamada IMLAB görüntü işleme kütüphanesini kullanacağız.

```c
// load the test sample
matrix_t *test = imread("..//data//faces//oscar.bmp");

// convert it to grayscale
matrix_t *gray = matrix_create(uint8_t, rows(test), cols(test), 1);
rgb2gray(test, gray);

// load the object model
struct haar_t *model = haar_read("../data/cascades/haarcascade_frontalface_alt.json");

// do the detection
vector_t *detections = haar_detector(gray, model, 20, 500, 1.15, 1.5f);

// merge the overlapping rectangles
detections = rectangle_merge(detections, 0.3, 1);

// draw the detections
printf("Drawing the %d detected objects...\n", length(detections));

struct rectangle_t *obj = vdata(detections, 0);
for(int i = 0; i < length(detections); i++)
{
    printf("obj[%03d]: %d %d %d %d\n", i, obj[i].x, obj[i].y, obj[i].width, obj[i].height);

    // set the thickness of the detected object 
    uint32_t thickness = maximum(1, sqrt(obj[i].width * obj[i].height) / 20);

    draw_rectangle(test, obj[i], RGB(50, 140, 100), 2 * thickness + 1);
    draw_rectangle(test, obj[i], RGB(80, 250, 255), thickness);
}

// write the marked objects as image
imwrite(test, "detected_objects.bmp");

// clear the allocations
matrix_free(&test);
matrix_free(&gray);
vector_free(&detections);
```
Verilen kodda ilk olarak test için kullanılacak görüntü okunmakta ve gri seviyeye çevrilmektedir. Ardından IMLAB kütüphanesinde yer alan ve Viola-Jones algoritmasında kullanılmak üzere eğitilen haar özniteliklerini saklayan `haar_t` modeli `haar_read` fonksiyonu yardımı ile içeri aktarılmıştır. İçeri aktarılan öznitelikler `haar_detector` fonksiyonu ile istenilen nesnelerin tespit edilmesi sağlanmıştır.`haar_detector` fonksiyonunun ilk parametresi testpitin yapılacağı imge, ikinci parametresi ise kullanılacak haar modelidir. Fonksiyonun üçüncü parametresi tespit edilecek en küçük, dördüncü parametresi ise en büyük nesne boyutunu piksel cinsinden ifade etmektedir. Fonksiyonun beşinci parametresi en küçük nesne ile en küçük nesne arasında arama yapılırken kullanılacak ölçekleme katsayısını göstermektedir. Fonksiyonun son parametresi ise pencere kaydırma adımlarını göstermektedir.

Aşağıda kodun örnek bir imge üzerindeki çıktısı verilmiştir. Görüldüğü üzere oldukça karmaşık bir imgede dahi algoritma çok az sayıda hatalı tespit üretmektedir.

![viola-jones yüz tespiti örnekleri][viola_jones_results]

**Referanslar**
* Viola, Paul, and Michael Jones. "Rapid object detection using a boosted cascade of simple features." Computer Vision and Pattern Recognition, 2001. CVPR 2001. Proceedings of the 2001 IEEE Computer Society Conference on. Vol. 1. IEEE, 2001.


[RESOURCES]: # (List of the resources used by the blog post)
[haar_wavelets]: /assets/post_resources/object_detection/haar_wavelets.png
[viola_jones_results]: /assets/post_resources/object_detection/oscar.png