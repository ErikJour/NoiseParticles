import * as Juce from './public/js/juce/javascript/index.js';
import * as THREE from 'three'
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls.js'
import GUI from 'lil-gui'

console.log("hi")

//CANVAS
const canvas = document.querySelector('canvas.webgl')


//------------------------------------------------------------------------------------------

//JUCE
window.__JUCE__.backend.addEventListener(
    "exampleEvent",
    (objectFromCppBackend) => {
        console.log(objectFromCppBackend);
    }
)

const nativeFunction = Juce.getNativeFunction("nativeFunction");

//------------------------------------------------------------------------------------------


// DEBUG
const gui = new GUI()

//------------------------------------------------------------------------------------------

//SCENE
const scene = new THREE.Scene()

//------------------------------------------------------------------------------------------

//LIGHT
const directionalLight = new THREE.DirectionalLight(0x00fffc, 1.0)
directionalLight.position.set(0, 1, 0)
scene.add(directionalLight)
directionalLight.intensity = 0.7
const ambientLight = new THREE.AmbientLight(0xffffff, 0.1);
scene.add(ambientLight);
ambientLight.intensity = 0.9



//------------------------------------------------------------------------------------------

//COLORS

const taliesenRed = new THREE.Color(204 / 255, 85 / 255, 0 / 255);


//MATERIAL
const objectMaterial = new THREE.MeshStandardMaterial({ color: taliesenRed })
objectMaterial.roughness = 0.3

const planeMaterial = new THREE.MeshStandardMaterial({ color: taliesenRed })
planeMaterial.roughness = 0.3

//----------------OBJECTS--------------------------------------------

// GRAIN OBJECT (WHITE NOISE)
const grainGroup = new THREE.Group();
scene.add(grainGroup);

const grainMaterial = new THREE.MeshPhongMaterial({
    color: 0xffffff,
    transparent: true,
    opacity: 0.1
});

const grainGeometry = new THREE.BoxGeometry(0.01, 0.01, 0.01);

function createGrain() {
    const mesh = new THREE.Mesh(grainGeometry, grainMaterial);
    
    const radius = 1.5; // Radius of the sphere around the main shape
    const theta = Math.random() * Math.PI * 2;
    const phi = Math.acos(2 * Math.random() - 1);
    
    mesh.position.set(
        radius * Math.sin(phi) * Math.cos(theta),
        radius * Math.sin(phi) * Math.sin(theta),
        radius * Math.cos(phi)
    );
    
    // Add velocity properties for animation
    mesh.velocity = new THREE.Vector3(
        (Math.random() - 0.5) * 0.01,
        (Math.random() - 0.5) * 0.01,
        (Math.random() - 0.5) * 0.01
    );
    
    return mesh;
}

function updateGrainObject(count) {
    grainGroup.clear();
    for (let i = 0; i < count; i++) {
        grainGroup.add(createGrain());
    }
    console.log("Sending grain count to backend:", count);
}



const noiseControl = { value: 0 };
gui.add(noiseControl, 'value', 0, 2000, 1)
    .name('Noise')
    .onChange((value) => {
        updateGrainObject(Math.floor(value)); // Update grain objects based on slider value
        sendLevelToCPP(Math.floor(value)); // Adjust this calculation as needed
    });

function sendLevelToCPP(noiseLevel) {
    const scaledNoiseLevel = noiseLevel / 2000;
    nativeFunction(['NoiseLevel', scaledNoiseLevel]);
}

//------------------------------------------------------------------------------------------

//Sizes
const sizes = {
    width: window.innerWidth,
    height: window.innerHeight
}


//CAMERA
const camera = new THREE.PerspectiveCamera(75, sizes.width / sizes.height)
camera.position.set(0.78, 0.96, 1.5)
scene.add(camera)

//CONTROLS
const controls = new OrbitControls(camera, canvas)
controls.enableDamping = true
controls.maxDistance = 8
controls.maxPolarAngle = Math.PI / 2 - 0.1


//RENDERER
const renderer = new THREE.WebGLRenderer({
    canvas: canvas,
    antialias: true
})

renderer.setSize(sizes.width, sizes.height)
renderer.render(scene, camera)

const oscillationAmplitude = 0.1875;
const oscillationFrequency = 0.5;

//ANIMATE
const animate = () => {
    requestAnimationFrame(animate);

   
    // Animate grain particles
    grainGroup.children.forEach(grain => {
        grain.position.add(grain.velocity);
        grain.material.opacity = 0.3 + Math.random() * 0.4;
    });
   
    controls.update();
    renderer.render(scene, camera);
};
animate();

//RESIZE
window.addEventListener('resize', () => {
    // Update sizes
    sizes.width = window.innerWidth;
    sizes.height = window.innerHeight;

    // Update camera
    camera.aspect = sizes.width / sizes.height;
    camera.updateProjectionMatrix();

    // Update renderer
    renderer.setSize(sizes.width, sizes.height);
    renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
});

