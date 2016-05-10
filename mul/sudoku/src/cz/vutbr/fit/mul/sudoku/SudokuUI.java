package cz.vutbr.fit.mul.sudoku;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.ResourceBundle;

import javafx.animation.Animation;
import javafx.animation.KeyFrame;
import javafx.animation.Timeline;
import javafx.application.Platform;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.ToggleButton;
import javafx.scene.input.KeyEvent;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.ColumnConstraints;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.RowConstraints;
import javafx.scene.media.Media;
import javafx.scene.media.MediaPlayer;
import javafx.stage.FileChooser;
import javafx.stage.FileChooser.ExtensionFilter;
import javafx.stage.Modality;
import javafx.stage.Stage;
import javafx.util.Duration;

public class SudokuUI implements Initializable{
	private final int HARD = 20;
	private final int MEDIUM = 35;
	private final int EASY = 50;
	private Main mainApp;
	private int currentPlayer;
	private File file;
	private ArrayList<MediaPlayer> mediaPlayers;
	private Sudoku sudoku;
	
	private ArrayList<Label> cells;
	private int currentCell;
	private int currentNumber;
	
    @FXML
    private GridPane boardGrid;
	
    @FXML
    private Label timeLabel;
	private Timeline timeline;
	protected int seconds;
	
	@Override
	public void initialize(URL location, ResourceBundle resources) {
		currentPlayer = 0;
		currentCell = 0;
		currentNumber = 1;
		file = null;
		mediaPlayers = new ArrayList<>();
		sudoku = new Sudoku();
		cells = new ArrayList<>(Collections.nCopies(Sudoku.SIZE2, null));
		
		for (int j = 0; j < 3; ++j) {
			  ColumnConstraints cc = new ColumnConstraints();
			  cc.setPercentWidth(100.0/3.0);
			  boardGrid.getColumnConstraints().add(cc);
			  RowConstraints rc = new RowConstraints();
			  rc.setPercentHeight(100.0/3.0);
			  boardGrid.getRowConstraints().add(rc);
		}
		
		ObservableList<Node> boxs = boardGrid.getChildren();
		for (Node box : boxs) {
			Integer boxRow = GridPane.getRowIndex(box); if(boxRow==null) boxRow = 0;
			Integer boxCol = GridPane.getColumnIndex(box); if(boxCol==null) boxCol = 0;
			GridPane gpBox = (GridPane) box;
			
			for (int j = 0; j < 3; ++j) {
				  ColumnConstraints cc = new ColumnConstraints();
				  cc.setPercentWidth(100.0/3.0);
				  gpBox.getColumnConstraints().add(cc);
				  RowConstraints rc = new RowConstraints();
				  rc.setPercentHeight(100.0/3.0);
				  gpBox.getRowConstraints().add(rc);
			}
			
			ObservableList<Node> labels = gpBox.getChildren();
			for (Node label : labels) {
				Integer cellRow = GridPane.getRowIndex(label); if(cellRow==null) cellRow = 0;
				Integer cellCol = GridPane.getColumnIndex(label); if(cellCol==null) cellCol = 0;
				int row = 3*boxRow + cellRow;
				int col = 3*boxCol + cellCol;
				int index = 9*row + col;
				cells.set(index, (Label) label);
				Label l = (Label) label; 
				l.setText("");
				
				EventHandler<MouseEvent> mouseHandler = new EventHandler<MouseEvent>() {
				    @Override
					public void handle(MouseEvent event) {
				    	if(event.getEventType() == MouseEvent.MOUSE_ENTERED){
				    		selectCell(cells.indexOf(event.getSource()));
				    	}else{
				    		if(event.getButton() == MouseButton.PRIMARY){
				    			assign(cells.indexOf(event.getSource()), currentNumber);
				    		}else if(event.getButton() == MouseButton.SECONDARY){
				    			assign(cells.indexOf(event.getSource()), 0);
				    		}
				    	}
				        event.consume();
				    }
				};
				
				label.addEventHandler(MouseEvent.MOUSE_ENTERED, mouseHandler);
				label.addEventHandler(MouseEvent.MOUSE_CLICKED, mouseHandler);
			}
			gpBox.setGridLinesVisible(true);
		}
		
		boardGrid.setFocusTraversable(true);
		boardGrid.addEventHandler(KeyEvent.KEY_PRESSED, new EventHandler<KeyEvent>() {
			@Override
			public void handle(KeyEvent event) {
				switch (event.getCode()) {
				case LEFT:
					if(currentCell%9==0) selectCell(currentCell+8);
					else selectCell(currentCell-1);
					break;
				case RIGHT:
					if(currentCell%9==8) selectCell(currentCell-8);
					else selectCell(currentCell+1);
					break;
				case UP:
					if((currentCell/9)%9==0) selectCell(currentCell+72);
					else selectCell(currentCell-9);
					break;
				case DOWN:
					if((currentCell/9)%9==8) selectCell(currentCell-72);
					else selectCell(currentCell+9);
					break;
				case DIGIT0:
				case NUMPAD0:
				case BACK_SPACE:
				case SPACE:
					assign(currentCell,0);
					break;
				case DIGIT1:
				case NUMPAD1:
					assign(currentCell,1);
					break;
				case DIGIT2:
				case NUMPAD2:
					assign(currentCell,2);
					break;
				case DIGIT3:
				case NUMPAD3:
					assign(currentCell,3);
					break;
				case DIGIT4:
				case NUMPAD4:
					assign(currentCell,4);
					break;
				case DIGIT5:
				case NUMPAD5:
					assign(currentCell,5);
					break;
				case DIGIT6:
				case NUMPAD6:
					assign(currentCell,6);
					break;
				case DIGIT7:
				case NUMPAD7:
					assign(currentCell,7);
					break;
				case DIGIT8:
				case NUMPAD8:
					assign(currentCell,8);
					break;
				case DIGIT9:
				case NUMPAD9:
					assign(currentCell,9);
					break;
				default:
					break;
				}
				event.consume();
			}
		});
		
		
		timeline = new Timeline(new KeyFrame(Duration.seconds(1), new EventHandler<ActionEvent>() {  
			@Override  
		    public void handle(ActionEvent event) {  
				seconds++;
				timeLabel.setText("Time: "+ seconds +"s");
		    }  
		}));  
		timeline.setCycleCount(Animation.INDEFINITE);    
		
		selectCell(0);
		newGameEasy(null);
	}   
	
	private void assign(int cell, int number){
		if(sudoku.isClue(cell)) return;
		sudoku.getPuzzle()[cell] = number;
		Label lcell = cells.get(cell);
		if(number == 0)
			lcell.setText("");
		else
			lcell.setText(String.valueOf(number));
		
		for(int j=0;j<=9;j++) lcell.getStyleClass().removeAll("number"+j);
		lcell.getStyleClass().add("number"+number);
		
		if(sudoku.isComplete()) complete();
	}
	
	private void complete() {
		timeline.stop();
		Stage dialogStage = new Stage();
		dialogStage.initModality(Modality.WINDOW_MODAL);
		Label label = new Label("Game completed in "+seconds+"s");
		dialogStage.setScene(new Scene(label, 300,100));
		dialogStage.show();
	}

	private void selectCell(int cell){
		cells.get(currentCell).getStyleClass().remove("currentcell");
		cells.get(cell).getStyleClass().add("currentcell");
		currentCell = cell;
	}
	
	@FXML
	void newGameEasy(ActionEvent event){
		newGame(EASY);
	}
	
	@FXML
	void newGameMedium(ActionEvent event){
		newGame(MEDIUM);
	}
	
	@FXML
	void newGameHard(ActionEvent event){
		newGame(HARD);
	}
	
    void newGame(int difficulty) {
		file = null;
		
		sudoku.generate(difficulty);
		sudoku.setSeconds(0);
		
		fillBoard();
	}
	
	@FXML
    void openGame(ActionEvent event) {
		timeline.stop();
		FileChooser fileChooser = new FileChooser();
		fileChooser.setTitle("Open sudoku");
		File openFile = fileChooser.showOpenDialog(mainApp.getPrimaryStage());
		if(openFile != null){ 
			file = openFile;
			try {
				FileInputStream fileIn = new FileInputStream(file);
				ObjectInputStream in = new ObjectInputStream(fileIn);
				try{
					sudoku = (Sudoku) in.readObject();
				} finally{
					in.close();
				}
			} catch (IOException | ClassNotFoundException e) {
				Stage dialogStage = new Stage();
				dialogStage.initModality(Modality.WINDOW_MODAL);
				Label label = new Label("Could not open game");
				dialogStage.setScene(new Scene(label, 300,100));
				dialogStage.show();
				return;
			}
			fillBoard();
			return;
		}
		timeline.play();
	}
	
	private void fillBoard() {
		for(int i = 0; i<Sudoku.SIZE2;++i){
			Label cell = cells.get(i);
			int value = sudoku.getPuzzle()[i];
			
			if(sudoku.isClue(i)){
				cell.getStyleClass().add("clue");
			}else{
				cell.getStyleClass().removeAll("clue");
			}
			
			if(value>0){
				cell.setText(String.valueOf(value));
			}else{
				cell.setText("");
			}
			
			for(int j=0;j<=9;j++) cell.getStyleClass().removeAll("number"+j);
			cell.getStyleClass().add("number"+value);
		}
		
		seconds = sudoku.getSeconds();
		timeLabel.setText("Time: "+seconds+"s");
		timeline.play();
	}
	
	private void save() {
		try {
			FileOutputStream fileOut = new FileOutputStream(file);
			ObjectOutputStream out = new ObjectOutputStream(fileOut);
			sudoku.setSeconds(seconds);
			try{
				out.writeObject(sudoku);
			}finally{
				out.close();
			}
		} catch (IOException e) {
			Stage dialogStage = new Stage();
			dialogStage.initModality(Modality.WINDOW_MODAL);
			Label label = new Label("Error on saving game");
			dialogStage.setScene(new Scene(label, 300,100));
			dialogStage.show();
		}
		
	}
	
    @FXML
    void saveGame(ActionEvent event) {
    	if(file == null){
    		saveGameAs(event);
    		return;
    	}
    	save();
    }

	@FXML
    void saveGameAs(ActionEvent event) {
		timeline.stop();
		FileChooser fileChooser = new FileChooser();
		fileChooser.setTitle("Save sudoku");
		File saveFile = fileChooser.showSaveDialog(mainApp.getPrimaryStage());
		if(saveFile != null){ 
			file = saveFile;
			save();
		}
		timeline.play();
    }

    @FXML
    void openMusic(ActionEvent event) {
    	timeline.stop();
    	FileChooser fileChooser = new FileChooser();
		fileChooser.setTitle("Choose audio files");
		fileChooser.getExtensionFilters().addAll(
		         new ExtensionFilter("Audio Files", "*.wav", "*.mp3", "*.aac")
		);
		List<File> selectedFiles = fileChooser.showOpenMultipleDialog(mainApp.getPrimaryStage());
		if (selectedFiles != null) {
			stopMusic(event);
			mediaPlayers.clear();
			currentPlayer = 0;
			for (File file : selectedFiles) {
				MediaPlayer mp = null;
				try{
					mp = new MediaPlayer(new Media(file.toURI().toString()));
				}catch(Exception mediaexception){
					Stage dialogStage = new Stage();
					dialogStage.initModality(Modality.WINDOW_MODAL);
					Label label = new Label("Error when loading "+file);
					dialogStage.setScene(new Scene(label, 500,100));
					dialogStage.show();
				}
				if(mp==null) continue;
				mp.setOnEndOfMedia(new Runnable() {
					@Override
					public void run() {
						System.out.println("music ended");
						currentPlayer = (currentPlayer+1)%mediaPlayers.size();
						mediaPlayers.get(currentPlayer).stop();
						mediaPlayers.get(currentPlayer).play();
					}
				});
				mediaPlayers.add(mp);
				mediaPlayers.get(currentPlayer).stop();
				mediaPlayers.get(currentPlayer).play();
			}	
		}
		timeline.play();
    }
    
    @FXML
    void stopMusic(ActionEvent event) {
    	if(!mediaPlayers.isEmpty()) {
			mediaPlayers.get(currentPlayer).stop();
		}
    }
	
    @FXML
    void openStyle(ActionEvent event) {
    	timeline.stop();
    	FileChooser fileChooser = new FileChooser();
		fileChooser.setTitle("Choose style file");
		fileChooser.getExtensionFilters().addAll(
		         new ExtensionFilter("Stylesheet", "*.css")
		);
		File openFile = fileChooser.showOpenDialog(mainApp.getPrimaryStage());
		if(openFile != null){
			resetStyle(event);
			mainApp.getPrimaryStage().getScene().getStylesheets().add(openFile.toURI().toString());
		}
		timeline.play();
	}
    

    @FXML
    void resetStyle(ActionEvent event) {
    	mainApp.getPrimaryStage().getScene().getStylesheets().clear();
		mainApp.getPrimaryStage().getScene().getStylesheets().add(getClass().getResource("application.css").toExternalForm());
    }
    
    @FXML
    void exit(ActionEvent event) {
		Platform.exit();
	}
    
    @FXML
    void toogleNumber(ActionEvent event) {
    	ToggleButton tb = (ToggleButton) event.getSource();
    	currentNumber = Integer.valueOf(tb.getText());
    }
    
	public void setMainApp(Main mainApp) {
		this.mainApp = mainApp;
	}
}
