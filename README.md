---
layout: post
title: Viola-Jones Yöntemi ile Nesne Tespiti
slug: viola-jones-object-detection-algorithm
author: Bahri ABACI
categories:
- Görüntü İşleme Uygulamaları
- Nesne Tespiti
references: "Rapid Object Detection Using a Boosted Cascade of Simple Features"
thumbnail: /assets/post_resources/object_detection/thumbnail.png
---

Nesne tespiti (object detection) karmaşık bir imge içerisinde spesifik/hedeflenen bir nesnenin var olup olmadığını, var ise hangi piksel bölgesi içerisinde yer aldığını belirleme işlemdir. Klasik yöntemlere göre çalışan bilgisayarlı görü uygulamalarında nesne tespiti; imge ön işleme (gri seviye dönüşüm, histogram eşitleme, eşikleme vb.) adımından sonra, nesne tanıma/sınıflandırma (bitki tanıma, yüz tanıma, plaka tanıma) adımından önce yapılan bir işlemdir. Nesne tespiti ile belirli bir nesneyi sınıflandırmak için eğitilmiş bir sistemin girdileri filtrelenerek, sınıflandırma sisteminin girdileri kontrol altına alınır. Bu sayede yüz tanıma için eğitilmiş akıllı bir sistemin yüz olmayan bölgelerde çalışması engellenir ve olası yanlış sınıflandırmaların önüne geçilir.

<!--more-->

Nesne tespiti bilgisayarlı görü camiasının uzun süredir üzerinde çalıştığı ve çok başarılı çıktılar ürettiği bir alandır. Bu yazımızda 2001 yılında Paul Viola ve Michael Jones tarafından önerilen ve özellikle yüz tespiti alanında hala sıklıkla tercih edilen Viola-Jones algoritmasını inceleyeceğiz. Viola-Jones algoritması her ne kadar nesne tespiti algoritması olsa da, konuyu yüz tespiti üzerinden anlatmak ve anlamanın daha kolay olacağını düşündüğümden anlatımlarımı bu alt başlık üzerinden yapacağım.

Viola-Jones algoritması bir pencere içerisinde (tipik pencere boyutu 24x24) yer alan piksel bloklarının bir biri ile olan ilşkisini öğrenme prensibi üzerine kurulmuştur. Bu ilişkiyi öğrenebilmek için aşağıda örnekleri gösterilen ve literatürde haar dalgacıkları olarak bilinen öznitelikler ve AdaBoost öğrenme algoritması kullanılmıştır.

![haar dalgacıkları][haar_wavelets]

### Öznitelik Çıkarımı
Görselleri verilen haar dalgacıklarda siyah pikseller negatif değerleri, beyaz pikseller ise pozitif değerleri göstermek için kullanılmıştır. Evrişim mantığına oldukça benzer şekilde; öznitelikler imge üzerinde gezdirilmekte ve beyaz bölge altında kalan piksellerin toplamı beyaz bölgenin katsayısı ile çarpılmakta, siyah bölge altında kalan piksellerin toplamı ile siyah bölgenin katsayısı çarpılmakta ve ardından bu sayılar toplanmaktadır. <img src="assets/post_resources/math//f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/> görüntüdeki piksel sayısı, <img src="assets/post_resources/math//63bb9849783d01d91403bc9a5fea12a2.svg?invert_in_darkmode" align=middle width=9.075367949999992pt height=22.831056599999986pt/> öznitelikteki toplanacak piksel sayısı olmak üzere bu işlemin karmaşıklığı <img src="assets/post_resources/math//de51e2f39424e8fdd146591e5179e2c1.svg?invert_in_darkmode" align=middle width=49.85618879999999pt height=24.65753399999998pt/> dır.

Bu işlemin her pikselde yüzlerce öznitelik için yapılması düşünüldüğünde işlemin oldukça uzun sürmesi beklenmektedir. Ancak Viola-Jones tarafından önerilen [tümlev imge]({% post_url 2019-02-17-tumlev-imge %}) (integral image) yaklaşımı özniteliklerin hesaplanması karmaşıklığını <img src="assets/post_resources/math//63bb9849783d01d91403bc9a5fea12a2.svg?invert_in_darkmode" align=middle width=9.075367949999992pt height=22.831056599999986pt/> öznitelik boyutundan arındırarak, <img src="assets/post_resources/math//e7a2f022962441f2be6dc8e70e837b4a.svg?invert_in_darkmode" align=middle width=40.78082744999999pt height=24.65753399999998pt/> seviyesine indirmektedir.

### Adaboost ile Öğrenme
Adaboost yöntemi ile öğrenme aşamasında ise şu adımlar izlenmektedir: İlk olarak ağırlık vektörü <img src="assets/post_resources/math//31fae8b8b78ebe01cbfbe2fe53832624.svg?invert_in_darkmode" align=middle width=12.210846449999991pt height=14.15524440000002pt/> her bir  imgeye eşit ağırlık verilerek ve seçilen sınıflandırıcılar kümesi <img src="assets/post_resources/math//2ad9d098b937e46f9f58968551adac57.svg?invert_in_darkmode" align=middle width=9.47111549999999pt height=22.831056599999986pt/>  boş küme ile ilklenir. Ardından her adımda, özniteliklerin her biri yüz  ve yüz olmayan binlerce imgeye (eğitim setine) uygulanır ve imgelerinin  bu filtrelere verdiği yanıtlar kaydedilir. Her bir öznitelik için bu  yanıtlar kullanılarak eğitim setini en küçük hata ile ayıran bir eşik  değeri bulunur ve en küçük hatayı üreten öznitelik seçilir. Seçilen bu  öznitelik h kümesine eklenir. Son aşamada belirlenen sınıflandırıcının  çıktıları kullanılarak doğru sınıflandırılan örneklerin ağırlığı  azaltılır, yanlış sınıflandırılan örneklerin ağırlığı artırılır.  Ardından yeni ağırlıklar ile yeni bir öznitelik belirleme işlemine  geçilir. Bu işlem belirli sayıda (<img src="assets/post_resources/math//2f118ee06d05f3c2d98361d9c30e38ce.svg?invert_in_darkmode" align=middle width=11.889314249999991pt height=22.465723500000017pt/> tane) sınıflandırıcı belirlenene  kadar devam edilir.

Algoritmanın çalışma prensibine ilişkin sözde kod aşağıdaki tabloda verilmiştir. 

* Eğitim setinde yer alan her olumlu örnek <img src="assets/post_resources/math//9fc20fb1d3825674c6a279cb0d5ca636.svg?invert_in_darkmode" align=middle width=14.045887349999989pt height=14.15524440000002pt/> için <img src="assets/post_resources/math//9a7769dfa8e59dcb07a8ed1e929c6071.svg?invert_in_darkmode" align=middle width=43.66906829999999pt height=21.18721440000001pt/>, her olumsuz örnek <img src="assets/post_resources/math//9fc20fb1d3825674c6a279cb0d5ca636.svg?invert_in_darkmode" align=middle width=14.045887349999989pt height=14.15524440000002pt/> için <img src="assets/post_resources/math//ae4a5afb811cb4bc45cce784bf8b2666.svg?invert_in_darkmode" align=middle width=43.66906829999999pt height=21.18721440000001pt/> etiketi atanır. <img src="assets/post_resources/math//f9c4988898e7f532b9f826a75014ed3c.svg?invert_in_darkmode" align=middle width=14.99998994999999pt height=22.465723500000017pt/> olumlu ve <img src="assets/post_resources/math//fb97d38bcc19230b0acd442e17db879c.svg?invert_in_darkmode" align=middle width=17.73973739999999pt height=22.465723500000017pt/> olumsuz örnek sayısı olmak üzere, ağırlık vektörü <img src="assets/post_resources/math//a9d8b0d9764becd9251fece31cce66be.svg?invert_in_darkmode" align=middle width=101.35994549999998pt height=27.77565449999998pt/> her bir imgeye eşit ağırlık verilerek ilklenir.

* Her iterasyon <img src="assets/post_resources/math//eed0bdd165d9485d37bb421e776b1b0c.svg?invert_in_darkmode" align=middle width=61.66067324999999pt height=22.465723500000017pt/> Ağırlıklar olasılık dağılımı olacak şekilde normalize edilir <p align="center"><img src="assets/post_resources/math//10c691388caad7054d9e555cbf460fd4.svg?invert_in_darkmode" align=middle width=133.98634754999998pt height=39.6869715pt/></p>

* Her <img src="assets/post_resources/math//6d22be1359e204374e6f0b45e318d561.svg?invert_in_darkmode" align=middle width=15.57562379999999pt height=22.831056599999986pt/> özniteliğini eğitim setine uygula ve kalan hatayı <p align="center"><img src="assets/post_resources/math//398ac0d7567f18cbf335e191bbfdcde5.svg?invert_in_darkmode" align=middle width=173.35430309999998pt height=36.6554298pt/></p> hesapla. 

* En küçük hatayı <img src="assets/post_resources/math//cbf94b0a9fb4d880661a1ab1549ca813.svg?invert_in_darkmode" align=middle width=11.638184249999991pt height=14.15524440000002pt/> veren <img src="assets/post_resources/math//d5231d154ec0bd2718bd57ba9a2c0698.svg?invert_in_darkmode" align=middle width=14.436907649999991pt height=22.831056599999986pt/> özniteliğini belirle. Doğru sınıflandırılan her örnek için <img src="assets/post_resources/math//d57105ddce52fa8c90efe0563925fe1b.svg?invert_in_darkmode" align=middle width=88.02852794999998pt height=37.80850590000001pt/> olmak üzere <p align="center"><img src="assets/post_resources/math//58d71134a36948372dceb3848de75cb0.svg?invert_in_darkmode" align=middle width=104.73277485pt height=16.1187015pt/></p> 

* Nihai sınıflandırıcıyı <img src="assets/post_resources/math//1c16bba367528a6e711bb1d14d4bf9e8.svg?invert_in_darkmode" align=middle width=91.98238334999998pt height=22.831056599999986pt/> olmak üzere
<p align="center"><img src="assets/post_resources/math//e5de236bc7189f07f31a422f2505b288.svg?invert_in_darkmode" align=middle width=285.4598802pt height=49.315569599999996pt/></p>
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
    uint32_t thickness = max(1, sqrt(obj[i].width * obj[i].height) / 20);

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