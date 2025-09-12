# Control de flujo y congestión en redes simuladas: un enfoque basado en retroalimentación

## Abstract
------------
En este trabajo se analiza el tráfico de red, el control de flujo y la congestión mediante simulaciones en OMNeT++. A partir de un modelo de colas básico, se incorporan limitaciones en la velocidad de transmisión y en la capacidad de los buffers para evaluar su impacto en dos casos de estudio con distintos cuellos de botella. Al variar el intervalo de generación de paquetes, se observan instancias de pérdidas de paquetes, acumulación de tráfico en los buffer, y un tiempo de procesamiento mayor. Esto, según la configuración, permitió identificar problemas del control de flujo y de congestión.

Para mitigar estos efectos, se diseñó un mecanismo de control basado en un feedback: el receptor no solo genera paquetes de feedback basados en su propio buffer, sino que también es capaz de retransmitir aquellos feedback que le llegan desde la red lo cual ajusta dinámicamente tasa de envío del generador. Se evaluó el funcionamiento y eficacia de esta solución con simulaciones, presentando curvas de tasa de pérdida y retardo en función del intervalo de paquetes. Finalmente, se hace una comparación de antes y después de ambos casos de estudio en donde observamos cuáles fueron las mejoras al sistema en cuanto rendimiento y confiabilidad.

## Introducción
---------------
El presente informe tiene como objetivo analizar el comportamiento de un sistema de colas en una red simulada utilizando la herramienta OMNeT++, en el marco de la asignatura Redes y Sistemas Distribuidos.

Realizando experimentos a partir de un modelo base compuesto por un generador de paquetes, una cola intermedia y un receptor final.

![Fig. 0 (Modelo Base)](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig0.png) *Fig. 0 (Modelo Base)*

En este caso, el *nodeTx* actúa como generador de paquetes. Está compuesto por dos componentes principales: un generador y una cola (buffer).

El generador se encarga de crear paquetes en intervalos de tiempo definidos por una distribución determinada.

La cola se encarga de gestionar el envío de los paquetes; en caso de que haya demoras, almacena temporalmente los paquetes hasta que puedan ser enviados.

El *nodeRx* actúa como receptor. También está compuesto por dos componentes: un sumidero (Sink) y una cola (buffer).

El receptor se encarga de procesar y consumir los paquetes a una velocidad determinada. La cola recibe los paquetes entrantes y los almacena temporalmente en el buffer hasta que el consumidor pueda procesarlos. Como veremos más adelante, este buffer tiene una capacidad limitada: si se llena, los nuevos paquetes que lleguen no podrán ser almacenados, y el buffer podría incluso descartar paquetes que ya contenía.

La cola intermedia (queue) se encarga de recibir y reenviar mensajes (paquetes). También cuenta con un buffer que permite almacenar temporalmente los paquetes cuando el flujo de datos en la red es demasiado rápido.

Sin embargo, este buffer tiene una capacidad limitada y puede llenarse, lo que provoca el descarte de nuevos paquetes entrantes.

El objetivo principal de estos experimentos es observar el impacto de estas restricciones en el comportamiento del sistema, identificar puntos de saturación, y caracterizar los fenómenos de pérdida de paquetes, aumento en los tiempos de procesamiento, y acumulación en los buffers. A partir de estos resultados, se busca establecer una relación directa entre los fenómenos observados y los conceptos teóricos de control de flujo y control de congestión, fundamentales en el diseño de protocolos de transporte robustos.

Se utilizarán diferentes variables durante los experimentos. 

El *generationInterval* es la distribución de tiempo de a cuanto se genera un paquete nuevo. El valor de esta variable se establecerá en 3 diferentes durante todas las simulaciones. exponential(0.1), exponential(0.5) y exponential(1)


## Primera Instancia de Simulación
-----------------------------------
### Caso 1: El receptor es el cuello de botella

![Fig. 1 (generationInterval=exponential(0.1))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig1.png) *Fig. 1 (generationInterval=exponential(0.1))*

![Fig. 2 (generationInterval=exponential(0.1))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig2.png) *Fig. 2 (generationInterval=exponential(0.1))*

En este experimento, con un intervalo de generación de paquetes establecido en una distribución exponential(0.1), el nodo (nodeRx) presenta un cuello de botella dado que el entre buffer del receptor y el destino final (Sink) hay un enlace más lento (0,5 Mbps). Esto, comparado con los 1 Mbps del resto de los enlaces. Como consecuencia, el buffer del receptor comienza a llenarse de forma lineal hasta alcanzar su capacidad máxima de 200 paquetes (alrededor de los 40 segundos desde el inicio de la simulación (Fig. 1)), y permanece saturado de manera continua. A causa de esto, el receptor no puede despachar los paquetes al Sink con la misma rapidez con la que los recibe, generando una acumulación progresiva en su buffer. 

Este comportamiento provoca un tiempo de procesamiento elevado que partir del segundo 80 (Figura 2) tras comenzar la simulación, el tiempo que la red y el receptor tardan en procesarlo supera los 40 s. Esto revela una acumulación severa en la cola de procesamiento, pues los paquetes permanecen en espera durante largos periodos antes de ser enviados.

Otra problemática grave, es la pérdida de paquetes; A partir del segundo 40, el sistema comienza a descartar alrededor de un paquete por segundo debido a la carencia de espacio en el buffer del receptor.

![Fig. 3 (generationInterval=exponential(0.1))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig3.png) *Fig. 3 (generationInterval=exponential(0.1))*

Sin embargo, se observaron diferencias notables en los resultados del mismo experimento al mantener constantes las tasas de transferencia entre los enlaces, pero modificando los valores del parámetro generationInterval.

En una primera instancia, se utilizó generationInterval = exponential(0.5), mientras que en la segunda se empleó generationInterval = exponential(1.0). Esta variación tuvo un impacto significativo en el comportamiento del sistema, como se analizará a continuación.

![Fig. 4 (generationInterval=exponential(0.5))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig4.png) *Fig. 4 (generationInterval=exponential(0.5))*
![Fig. 5 (generationInterval=exponential(0.5))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig5.png) *Fig. 5 (generationInterval=exponential(0.5))*
![Fig. 6 (generationInterval=exponential(1))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig6.png) *Fig. 6 (generationInterval=exponential(1))*
![Fig. 7 (generationInterval=exponential(1))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig7.png) *Fig. 7 (generationInterval=exponential(1))*

Como se observa en la Fig. 4, la cantidad máxima de paquetes presentes en el buffer del receptor es de 3 paquetes, un valor ínfimo en comparación con el observado en la Fig. 1 dentro del mismo lapso de tiempo. Esta diferencia se refleja directamente en el tiempo de procesamiento por paquete, como se muestra en la Fig. 5, donde se evidencia que, en el peor de los casos, un paquete no supera el segundo de procesamiento.

De manera similar a lo anteriormente descrito, al incrementarse el intervalo de generación de paquetes, los resultados muestran una disminución aún más pronunciada. En el caso de la Fig 6, la cantidad de paquetes presentes en el buffer del receptor se mantiene constante en 1, lo que se traduce en un tiempo de procesamiento por paquete inferior al segundo (Fig. 7), reflejando un comportamiento altamente eficiente del sistema bajo estas condiciones.

Cabe destacar que, en ninguno de los dos últimos casos, el buffer del receptor alcanza su capacidad máxima. Como consecuencia, no se produce pérdida de paquetes, lo que evidencia un comportamiento estable y controlado del sistema bajo estas configuraciones. Sin embargo, el desempeño y la eficiencia global del sistema se ven afectados, ya que la cantidad total de paquetes enviados y recibidos es considerablemente menor en comparación con los escenarios donde sí se produce pérdida de paquetes, todo ello dentro del mismo intervalo de tiempo.

El primer escenario donde se utiliza un intervalo de generación de paquetes bajo (generationInterval=exponential(0.1)) representa un problema clásico de *control de flujo*. En redes, el control de flujo se refiere a los mecanismos que regulan la velocidad a la que un emisor transmite datos, de manera que el receptor no se vea desbordado. En este caso, la falta de dicha regulación hace que el receptor reciba más de lo que puede procesar y su buffer se sature, generando pérdidas de paquetes.


### Caso 2: Cuello de botella en la red

En este segundo caso, el enfoque de la problemática se encuentra en el enlace que conecta el buffer intermedio (la cola intermedia) con el receptor, configurado con una tasa de transferencia de 0.5 Mbps, mientras que el enlace hacia el Sink es de 1 Mbps. Con el generationInterval = exponential(0.1). El haber cambiado estas tasas de transferencia generó un efecto diferente al anterior.

![Fig. 8](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig8.png) *Fig. 8*

Observemos que el buffer de la red comienza a llenarse de forma lineal hasta alcanzar su capacidad máxima de 200 paquetes, alrededor de los 40 segundos desde el inicio de la simulación (Fig.8), y permanece saturada de manera continua.

Debido a esto la red comienza a congestionarse provocando que el tiempo de envío hacia el receptor (NodeRx) se vea afectado drásticamente.

![Fig. 9 (generationInterval=exponential(0.1))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig9.png) *Fig. 9 (generationInterval=exponential(0.1))*

![Fig. 10 (generationInterval=exponential(0.1))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig10.png) *Fig. 10 (generationInterval=exponential(0.1))*

A partir del segundo 80 del inicio de la simulación, se comienza a evidenciar una disminución en el desempeño del sistema. En la Fig 9 se observa que el tiempo necesario para que un paquete sea procesado y enviado desde la red hasta el receptor supera los 40 segundos. Esta situación se termina agravando en lo que muestra la Fig. 10, donde se comprueba la persistencia del retardo en el envío de paquetes, provocado por la propagación de la congestión a lo largo de la red.

De manera análoga a cuando la red presentaba problemas de flujo en el Caso 1, se observa un descarte de paquetes a un ritmo aproximado de uno por segundo pero en este caso en el lado del buffer de la red (Fig. 11).

![Fig. 11 (generationInterval=exponential(0.1))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig11.png) *Fig. 11 (generationInterval=exponential(0.1))*

Al repetir los experimentos variando los intervalos de generación de paquetes, se obtuvieron resultados de desempeño muy similares a los observados en el Caso 1, donde también se presentaban problemas de flujo. Al generarse menos paquetes por unidad de tiempo, la probabilidad de que un búfer (en este caso, el de la red) llegue a saturarse es prácticamente nula. Esto se evidencia en la Fig.12, donde el número máximo de paquetes en el búfer no supera los 3. Como consecuencia, no se produce congestión en la red, lo que se refleja en la Fig.13, donde el tiempo de procesamiento y envío de cada paquete es notablemente bajo. Cabe destacar que, cuando el intervalo de generación de paquetes sigue una distribución exponencial con media 1 (exponential(1)), el comportamiento observado también resulta análogo al del Caso 1.

![Fig. 12 (generationInterval=exponential(0.5))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig12.png) *Fig. 12 (generationInterval=exponential(0.5))*

![Fig. 13 (generationInterval=exponential(0.5))](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig13.png) *Fig. 13 (generationInterval=exponential(0.5))*

El primer escenario donde se utiliza un intervalo de generación de paquetes bajo (generationInterval=exponential(0.1)), el resultado analítico corresponde a un problema de *control de congestión*. A diferencia del control de flujo, el control de congestión se encarga de prevenir la sobrecarga dentro de la red misma, es decir, cuando los nodos intermedios (como la Queue) se ven saturados por un tráfico excesivo que no pueden procesar o reenviar con suficiente rapidez. Aquí, el cuello de botella en la red produce acumulación en la cola intermedia y pérdida de paquetes aun cuando el receptor está disponible.


## Modificación de la simulación para gestión de flujo y congestión
--------------------------------------------------------------------
Con el objetivo de simular una situación más realista en la comunicación entre emisor, red y receptor, se propone una modificación en la arquitectura del sistema implementado en OMNeT++. La finalidad de esta modificación es permitir el control del flujo y la gestión de congestión mediante retroalimentación entre el receptor y el emisor.
Para ello, se realizaron los siguientes cambios en la red y los módulos involucrados:

- **Módulo traTx (Transmisor):**
Se rediseñó su interfaz para incluir una puerta inout, la cual permite tanto recibir paquetes generados por el módulo gen, como también aceptar paquetes de retroalimentación provenientes del receptor a través del módulo de la red (queue1). De esta forma, traTx puede adaptar su comportamiento en función del estado actual de la red y del receptor.


- **Módulo traRx (Receptor):**
Se amplió su funcionalidad para que no solo reciba paquetes desde la red, sino que también tenga capacidad de generar paquetes de retroalimentación (feedback) con información relevante sobre el estado de recepción o posibles eventos de congestión. Estos paquetes se envían hacia el emisor a través de queue1.


- **Red (Network):**
Se actualizó la estructura de la red para soportar la bidireccionalidad de la comunicación. Esto permite que los paquetes de control (feedback) generados por el receptor puedan viajar de vuelta al emisor, cerrando así el ciclo de retroalimentación. Los módulos de cola (queue0 y queue1) permiten enrutar correctamente los paquetes en ambas direcciones.


Con esta modificación, se establece un mecanismo básico de control de flujo que permitirá, por ejemplo, adaptar la tasa de generación de paquetes en función de la congestión detectada en el receptor. Este enfoque proporciona una base para evaluar estrategias de control dinámico del tráfico en redes de comunicación.

![Fig. 14](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig14.png) *Fig. 14* 
![Fig. 15](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig15.png) *Fig. 15* 
![Fig. 16](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig16.png) *Fig. 16*


#### **Estrategia de Control de Flujo y Congestión**
Nuestra solución al problema de congestión y control de flujo se basa en un mecanismo de retroalimentación (feedback) que permite al emisor ajustar dinámicamente su tasa de envío de paquetes, en función del estado del receptor y de la red.

#### **Control de flujo en el receptor**
Cuando el buffer del receptor alcanza el 80% de su capacidad, se genera un paquete de retroalimentación dirigido al emisor. Este paquete contiene una bandera (slowdown_by_receiver) activada, que indica la necesidad de reducir la velocidad de envío para evitar la saturación del receptor.

#### **Control de congestión en la red**
De forma similar, cuando la red detecta congestión (por ejemplo, si uno de los buffers intermedios alcanza también el 80% de ocupación), se envía un paquete de feedback a través del receptor. El receptor actúa entonces como reenrutador de este mensaje, asegurando que el emisor reciba una notificación con la bandera (slowdown_by_network) activada.

#### **Función del receptor**
El receptor, por tanto, debe ser capaz tanto de generar paquetes de retroalimentación propios como de redirigir los generados por la red, asegurando así una vía de comunicación constante hacia el emisor sobre el estado del sistema.

#### **Comportamiento del emisor ante la retroalimentación**
Al recibir un paquete de retroalimentación con alguna de las banderas activadas, el emisor ajusta la planificación de los próximos paquetes, incrementando un escalar de retardo en 0.1 por cada feedback recibido. Este escalar se aplica como un aumento en el tiempo entre la generación de nuevos paquetes, dando tiempo al sistema para procesar los paquetes en tránsito y evitar la pérdida de datos por sobrecarga.

#### **Mecanismo de recuperación**
Para evitar que una acumulación excesiva de mensajes de retroalimentación degrade indefinidamente el rendimiento, se implementó un mecanismo de recuperación dinámica. Se definió una constante temporal de 0.5 segundos, y cada vez que se va a generar un nuevo paquete, se verifica cuánto tiempo ha transcurrido desde el último feedback recibido. Si ha pasado más de 0.5 segundos sin nuevos feedbacks, el escalar se reduce, lo que acorta el intervalo entre paquetes y permite recuperar progresivamente la tasa de envío. De esta manera, el sistema logra una planificación adaptativa, que responde tanto a la congestión como a su alivio, equilibrando eficiencia y confiabilidad.


## **Resultados Esperados con la Solución Propuesta**
-------------------------------------------------

La implementación del mecanismo de retroalimentación tiene como objetivo mitigar los efectos negativos observados en la simulación original, en la que se identificaron los siguientes problemas principales:

- Pérdida de paquetes tanto en la red como en el receptor, debido a la saturación de los buffers.

- Retrasos significativos en la atención de los paquetes, ya que estos permanecían encolados durante largos períodos antes de ser procesados.

- Diferencia considerable entre la cantidad de paquetes enviados y los recibidos, lo que refleja ineficiencia en la transmisión de datos.


Con la solución propuesta, basada en un control dinámico del flujo a través de paquetes de feedback, se espera lo siguiente:

1. **Reducción significativa de la pérdida de paquetes:**
Al permitir que el emisor desacelere la generación de paquetes ante señales de congestión, los buffers intermedios y del receptor tendrán mayor capacidad para procesar lo que reciben, evitando desbordamientos.


2. **Mejora en los tiempos de procesamiento y entrega:**
Al espaciar la generación de paquetes en momentos de alta carga, se evita que estos queden en cola durante mucho tiempo. Como resultado, los paquetes deberían ser atendidos más rápidamente una vez que ingresan al sistema.


3. **Mayor correspondencia entre paquetes enviados y recibidos:**
Un sistema que regula su tasa de envío de forma adaptativa debería lograr una mayor eficiencia en la transmisión, reduciendo la brecha entre los paquetes que se envían y los que efectivamente llegan al destino.


4. **Comportamiento adaptativo a lo largo del tiempo:**
Gracias al mecanismo de recuperación del escalar de planificación, el sistema no queda permanentemente limitado por condiciones pasadas de congestión. Esto permite que el rendimiento se recupere cuando las condiciones de la red mejoran.


En resumen, se espera que el sistema modificado alcance un equilibrio entre rendimiento y confiabilidad, ajustando dinámicamente su comportamiento según las condiciones actuales de carga en la red y en el receptor. Esta capacidad de adaptación es clave para evitar pérdidas, mejorar la latencia y asegurar una comunicación más estable y eficiente.


## **Segunda Instancia de Simulación**
----------------------------------
### **Caso 1: El receptor es el cuello de botella**

![Fig. 17](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig17.png) *Fig. 17*
![Fig. 18](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig18.png) *Fig. 18*
![Fig. 19](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig19.png) *Fig. 19*
![Fig. 20](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig20.png) *Fig. 20*
![Fig. 21](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig21.png) *Fig. 21*
![Fig. 22](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig22.png) *Fig. 22*


El tiempo que transcurre desde que un paquete es generado hasta que es procesado por el receptor (Fig. 17) es similar al tiempo que ese mismo paquete permanece en el nodo transportTx (Fig. 19). Este nodo, implementado con un algoritmo de planificación dinámica, ajusta el tiempo de envío en función del estado del sistema. La similitud en los tiempos indica que el retardo principal del paquete ocurre en el nodo emisor y no en el receptor, lo que significa que la transmisión entre emisor y receptor es eficiente y rápida, manteniendo un flujo controlado.

En contraste, en la figura 20 —correspondiente a la versión sin algoritmo— se observa que el tiempo de vida de un paquete hasta ser procesado por el receptor llega a los 40 segundos y se mantiene oscilando en ese valor. Esto sucede porque el receptor no logra procesar nuevos paquetes a tiempo, generando un estancamiento. En cambio, con la versión que incorpora el algoritmo, sí se puede procesar nuevos paquetes continuamente, por lo que el tiempo no se estabiliza en ese valor, sino que varía conforme se atienden los paquetes más rápidamente.

Además, el gráfico de ocupación del buffer del receptor (Fig. 18) muestra que, con el algoritmo, este se mantiene oscilando alrededor del 80%, sin llegar a saturarse. Por el contrario, en la versión sin algoritmo (Fig. 21), el buffer del receptor se llena completamente. 

Esta diferencia también se evidencia al comparar el estado del buffer del emisor. En la versión sin algoritmo, el emisor apenas acumula 60-70 paquetes en su buffer, mientras que en la nueva versión (Fig. 22), su ocupación alcanza hasta 800 paquetes. Esto sucede porque el emisor, al recibir feedbacks que indican congestión o lentitud en el receptor, retiene los paquetes por más tiempo, dándole espacio al receptor para procesar los suyos sin saturarse.

Este comportamiento evidencia que, aunque el emisor aumenta temporalmente su carga de trabajo, se logra un balance global del sistema, evitando pérdidas y manteniendo un flujo continuo. A continuación, observaremos un comportamiento análogo para el caso de congestión en la red, donde aplicaremos la misma lógica de feedback adaptativo.


### **Caso 2: Cuello de botella en la red**

![Fig. 23](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig23.png) *Fig. 23*
![Fig. 24](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig24.png) *Fig. 24*
![Fig. 25](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig25.png) *Fig. 25*
![Fig. 26](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig26.png) *Fig. 26*
![Fig. 27](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig27.png) *Fig. 27*

Podemos observar que el *tiempo desde la creación de un paquete hasta que es desencolado de la red disminuye progresivamente* (Fig. 23). Esto se debe a que, alrededor del segundo 40, al alcanzarse el 80% de ocupación del buffer de la red, se genera un paquete de feedback indicando congestión. A partir de ese momento, el sistema comienza a alternar el procesamiento entre paquetes nuevos y los que ya se encontraban en cola, permitiendo liberar la acumulación de paquetes antiguos. Esta dinámica no se presenta en el *caso 2 de la primera versión*, donde no se contaba con este mecanismo de control.

Posteriormente, como se observa en la Fig. 24, el buffer de la red muestra una oscilación controlada: su nivel de ocupación sube y baja, pero sin sobrepasar el umbral crítico del 80%. Esto evidencia que el sistema logra mantenerse en un estado de equilibrio, evitando la saturación.

En comparación, en el *caso 1* (Fig. 25) — donde no se implementa el algoritmo de control de congestión— el tiempo entre la creación y el procesamiento de los paquetes en la red aumenta continuamente hasta estabilizarse cuando el buffer se llena por completo. En este escenario, la red no logra vaciar adecuadamente su cola, lo que conlleva pérdidas y retrasos significativos.

Adicionalmente, en el *caso 2* (Figs. 26 y 27) se observa que se logra *procesar una mayor cantidad de paquetes generados*, lo cual se puede verificar al comparar los gráficos de ocupación de buffer con y sin el algoritmo. En este nuevo enfoque:

- El **receptor procesa los paquetes casi instantáneamente**, manteniendo su buffer con una ocupación cercana a cero.

- La **red reduce su nivel de ocupación de forma notable**, gracias al control adaptativo del flujo.

- El **emisor incrementa la ocupación de su buffer**, ya que debe gestionar dinámicamente el tiempo de planificación de nuevos paquetes. Como consecuencia, muchos paquetes quedan temporalmente en cola de espera antes de ser enviados, pero sin provocar pérdidas ni congestión a lo largo del sistema.



## **Conclusión**
---

En este informe se presentó un algoritmo de control de flujo y congestión basado en el uso de paquetes de feedback, diseñado para resolver la pérdida de paquetes en dos escenarios: congestión en la red y cuello de botella en el receptor. El algoritmo funciona de la siguiente manera:

- Cuando el receptor detecta que su buffer supera un umbral del 80%, genera un paquete de feedback con una bandera *slowdown_by_receiver* en true, que se envía al emisor.


- De igual forma, si la red detecta congestión (también al superar el 80% de ocupación), genera un feedback que es redirigido por el receptor hacia el emisor, con la bandera *slowdown_by_red* en true.


- Al recibir estos feedbacks, el emisor ajusta dinámicamente el tiempo de planificación de los paquetes, aumentando un escalar que retrasa la generación de nuevos paquetes, permitiendo que tanto la red como el receptor puedan procesar los paquetes pendientes y evitando la saturación.


- Además, para evitar una desaceleración excesiva, si ha pasado más de 0.5 segundos desde el último feedback, este escalar comienza a disminuir progresivamente, reactivando el envío.


Gracias a esta lógica adaptativa, se observó que no se registraron pérdidas de paquetes en ambos casos, a diferencia de la versión sin algoritmo, donde los paquetes eran descartados tanto en la red como en el receptor. Esto se evidencia en los gráficos de carga ofrecida vs. carga útil (Fig. 28), donde se trazaron cuatro líneas: dos correspondientes a los casos sin algoritmo y dos con el nuevo algoritmo. Las líneas de la versión sin algoritmo colapsan a medida que se incrementa la tasa de generación de paquetes, reflejando que no se logra mantener la misma tasa de recepción, lo cual indica congestión y pérdida de información.

En cambio, las líneas de la versión con algoritmo se acercan mucho más al comportamiento ideal, mostrando que se reciben más paquetes por segundo en proporción a los que se envían, aunque con una leve penalización debido a la sobrecarga que introduce el tratamiento de feedbacks.

No obstante, aún queda un aspecto por mejorar. Si bien se evita la pérdida de paquetes, los gráficos de ocupación de buffer muestran que el emisor acumula una gran cantidad de paquetes pendientes de ser enviados, llegando incluso a valores muy elevados. Esto indica que se siguen generando paquetes incluso cuando la capacidad de procesamiento de la red y el receptor ya están al límite.

Como trabajo futuro, sería deseable incorporar una lógica que limite la generación de nuevos paquetes en función de la cantidad efectiva que puede ser desencolada por el sistema, para así lograr un equilibrio total entre generación, transmisión y recepción de paquetes.


![Fig. 28](https://bitbucket.org/redes-famaf/redes25lab3g09/raw/a3992cf6669ea6f45a49325a16d2d17702e73036/Images/fig28.png) *Fig. 28*
